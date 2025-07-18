/**
 * @file
 * Describes a strided prefetcher.
 */

#ifndef __MYLIB_MEM_CACHE_NEXT_LINE_STRIDE_HH__
#define __MYLIB_MEM_CACHE_NEXT_LINE_STRIDE_HH__

#include "mem/cache/prefetch/queued.hh"
#include "params/NextLinePrefetcher.hh"


namespace gem5
{
namespace prefetch
{

class NextLine : public Queued
{
private:
    /* data */
    int degree;
public:
    NextLine(const NextLinePrefetcherParams& params);

    void calculatePrefetch(
        const PrefetchInfo &pfi,
        std::vector<AddrPriority> &addresses,
        const CacheAccessor &cache
    ) override;
};

} // namespace prefetch
}

#endif