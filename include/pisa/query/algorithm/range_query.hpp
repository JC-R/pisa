#pragma once

#include "query/queries.hpp"
#include "topk_queue.hpp"

namespace pisa {

template <typename QueryAlg>
struct range_query {

    range_query(topk_queue& topk)
        : m_topk(topk) {}

    template <typename CursorRange>
    uint64_t operator()(CursorRange &&cursors, uint64_t max_docid, size_t range_size)
    {
        if (cursors.empty()) {
            return 0;
        }

        for (size_t end = range_size;
             end + range_size <= max_docid; end += range_size) {
            process_range(cursors, end);
        }
        process_range(cursors, max_docid);

        m_topk.finalize();
        return m_topk.topk().size();
    }

    std::vector<std::pair<float, uint64_t>> const &topk() const { return m_topk.topk(); }

    template <typename CursorRange>
    void process_range(CursorRange &&cursors, size_t end)
    {

        QueryAlg query_alg(m_topk);
        query_alg(cursors, end);
    }

   private:
    topk_queue& m_topk;
};

} // namespace pisa
