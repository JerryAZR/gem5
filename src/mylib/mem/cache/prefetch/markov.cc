#include "mylib/mem/cache/prefetch/markov.hh"

#include "debug/MarkovPrefetch.hh"
#include "markov.hh"

#include <algorithm>

namespace gem5
{
namespace prefetch
{

static std::hash<uint64_t> stdHash;

static uint64_t hash64shift(long key)
{
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;
}

static uint64_t combineHash(uint64_t h1, uint64_t h2) {
    uint64_t r1 = hash64shift(h1);
    uint64_t r2 = hash64shift(h2);
    return r1 ^ r2;
}

Markov::Markov(const MarkovPrefetcherParams &params)
:   Queued(params),
    historyLength(params.history_length),
    tableSize(params.table_size),
    numNextCandidates(params.num_next_candidates),
    prefetchDegree(params.prefetch_degree),
    lastBlock(0)
{
    DPRINTF(MarkovPrefetch, "MarkovPrefetcher initialized with "
                      "historyLength=%u, tableSize=%u, numNextCandidates=%u, "
                      "prefetchDegree=%u\n",
            historyLength, tableSize, numNextCandidates, prefetchDegree);

    fatal_if(historyLength < 1, "Fetch history must be positive.");
    accessHistory = std::deque<FetchHistoryEntry>(historyLength, FetchHistoryEntry(0, 0, 0));
    markovTable = std::unordered_map<uint64_t, MarkovNode>();
}

Markov::Transition::Transition(Addr target, unsigned confidence)
: target(target), confidence(confidence) {}

Markov::FetchHistoryEntry::FetchHistoryEntry(Addr addr, Addr pc, Addr stride)
: addr(addr), pc(pc), stride(stride) {}

uint64_t Markov::FetchHistoryEntry::getHashCode() const {
    return hash64shift(addr);
}

void Markov::calculatePrefetch(
    const PrefetchInfo &pfi,
    std::vector<AddrPriority> &addresses,
    const CacheAccessor &cache
) {
    if (lastBlock == blockAddress(pfi.getAddr())) return;
    lastBlock = blockAddress(pfi.getAddr());
    // Update history and get keys
    uint64_t prevKey = generateKeyFromHistory();

    if (accessHistory.size() >= historyLength) accessHistory.pop_back();
    Addr pc = pfi.hasPC() ? pfi.getPC() : 0;
    Addr addr = blockAddress(pfi.getAddr());
    Addr stride = addr - accessHistory.front().addr;
    accessHistory.emplace_front(addr, pc, stride);

    uint64_t currKey = generateKeyFromHistory();

    // Update model
    DPRINTF(MarkovPrefetch, "Updating key %x -> address %x.\n", prevKey, addr);
    updateMarkovTable(prevKey, addr);

    // Get prediction
    if (markovTable.find(currKey) != markovTable.end()) {
        DPRINTF(MarkovPrefetch, "Markov model found for key %x.\n", currKey);
        std::sort(markovTable[currKey].begin(), markovTable[currKey].end(),
        [](const Transition &a, const Transition &b) {
            return a.confidence > b.confidence;
        });
        for (size_t i = 0; i < prefetchDegree; i++) {
            if (markovTable[currKey].size() <= i || markovTable[currKey][i].confidence < confidenceThreshold) {
                break;
            } else if (markovTable[currKey][i].target != blockAddress(addr)) {
                // Schedule prefetch
                addresses.emplace_back(markovTable[currKey][i].target, 0);
            }
        }
    } else {
        // Fetch next line as a fallback
        // Addr pfAddr = blockAddress(addr) + blkSize;
        // addresses.emplace_back(pfAddr, 0);
        // DPRINTF(MarkovPrefetch, "No Markov model found for key %x. Default to next-line (%x).\n", currKey, pfAddr);
    }
}

uint64_t Markov::generateKeyFromHistory() const
{
    uint64_t hash = 0x165667b1d3a2646c;
    for (const FetchHistoryEntry &entry : accessHistory)
    {
        hash = combineHash(hash, entry.getHashCode());
    }

    return hash % tableSize;
}

void Markov::updateMarkovTable(uint64_t key, Addr addr)
{
    if (markovTable.find(key) != markovTable.end()) {
        // Confidence decay
        for (Transition &entry : markovTable[key]) {
            if (entry.target != addr && entry.confidence > 0) entry.confidence--;
        }
        // Update existing entry
        for (Transition &entry : markovTable[key]) {
            if (entry.target == addr) {
                if (entry.confidence < confidenceMax) entry.confidence++;
                return;
            }
        }

        if (markovTable[key].size() < numNextCandidates) {
            // Add new entry
            markovTable[key].emplace_back(addr, confidenceInit);
        } else {
            // Ignore or replace existing entry
            auto it = std::min_element(
                markovTable[key].begin(), markovTable[key].end(),
                [](const Transition &a, const Transition &b) {
                    return a.confidence < b.confidence;
                });

            if (it->confidence < confidenceThreshold) {
                it->confidence = confidenceInit;
                it->target = blockAddress(addr);
            }
        }

    } else {
        // Add new entry
        markovTable[key] = MarkovNode();
        markovTable[key].emplace_back(blockAddress(addr), confidenceThreshold);
    }

}

}
}