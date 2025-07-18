#include "mylib/mem/cache/prefetch/next_line.hh"

namespace gem5
{
namespace prefetch
{

NextLine::NextLine(const NextLinePrefetcherParams& params)
: Queued(params), degree(params.degree)
{
    fatal_if(degree < 0, "Prefetch degree must be non-negative.");
}

void NextLine::calculatePrefetch(
    const PrefetchInfo &pfi,
    std::vector<AddrPriority> &addresses,
    const CacheAccessor &cache
) {
    if (pfi.hasPC() == false) return;

    Addr base_addr = blockAddress(pfi.getAddr());

    for (int i = 1; i <= degree; ++i) {
        Addr next_line = base_addr + i * blkSize;
        addresses.push_back(AddrPriority(next_line, 0));
    }
}

} // namespace prefetch
} // namespace gem5
