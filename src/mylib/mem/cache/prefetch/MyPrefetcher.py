from m5.objects import QueuedPrefetcher
from m5.params import *


class NextLinePrefetcher(QueuedPrefetcher):
    type = "NextLinePrefetcher"
    cxx_class = "gem5::prefetch::NextLine"
    cxx_header = "mylib/mem/cache/prefetch/next_line.hh"

    # Number of lines to prefetch ahead (1 means "next-line prefetcher")
    degree = Param.Int(1, "Number of lines to prefetch ahead")

class MarkovPrefetcher(QueuedPrefetcher):
    type = "MarkovPrefetcher"
    cxx_class = "gem5::prefetch::Markov"
    cxx_header = "mylib/mem/cache/prefetch/markov.hh"

    # How many past accesses to consider
    history_length = Param.Unsigned(2, "Number of past accesses to use for prediction")

    # Maximum number of entries in the Markov table
    table_size = Param.Unsigned(512, "Max number of entries in the Markov transition table")

    # How many next candidates (most frequent successors) to store per key
    num_next_candidates = Param.Unsigned(4, "Number of next address deltas to track per key")

    # How many addresses to prefetch per access (follow-up steps)
    prefetch_degree = Param.Unsigned(2, "Number of sequential predictions to prefetch")
