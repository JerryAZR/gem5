#ifndef __MYLIB_MEM_CACHE_PREFETCH_MARKOV_HH__
#define __MYLIB_MEM_CACHE_PREFETCH_MARKOV_HH__

#include <deque>
#include <unordered_map>
#include <vector>

#include "mem/cache/prefetch/queued.hh"
#include "params/MarkovPrefetcher.hh"

namespace gem5
{

namespace prefetch
{

class Markov : public Queued
{
  public:
    Markov(const MarkovPrefetcherParams &params);

    void calculatePrefetch(
        const PrefetchInfo &pfi,
        std::vector<AddrPriority> &addresses,
        const CacheAccessor &cache
    ) override;

  private:
    /** Markov Table */
    struct Transition {
        Addr target;
        unsigned confidence;

        Transition(Addr target, unsigned confidence);
    };

    struct FetchHistoryEntry {
        Addr addr;
        Addr pc;
        Addr stride;  // Optional for now, can be filled later if needed

        FetchHistoryEntry(Addr addr, Addr pc, Addr stride);
        uint64_t getHashCode() const;
    };

    /** Parameters */
    const unsigned historyLength;
    const unsigned tableSize;
    const unsigned numNextCandidates;
    const unsigned prefetchDegree;
    const unsigned confidenceThreshold = 2;
    const unsigned confidenceMax = 16;
    const unsigned confidenceInit = confidenceMax >> 1;

    Addr lastBlock;

    /** History of past addresses (per stream). Simple global history for now. */
    std::deque<FetchHistoryEntry> accessHistory;

    using MarkovNode = std::vector<Transition>;
    std::unordered_map<uint64_t, MarkovNode> markovTable;

    /** Generate a key for the Markov table based on recent accesses. */
    uint64_t generateKeyFromHistory() const;

    /** Insert a new transition into the table. */
    void updateMarkovTable(uint64_t key, Addr addr);
};

} // namespace prefetch
} // namespace gem5

#endif // __MEM_CACHE_PREFETCH_BASIC_MARKOV_HH__
