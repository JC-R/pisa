#pragma once

#include "query/queries.hpp"
#include "scorer/bm25.hpp"
#include <vector>

namespace pisa {

template <typename Index, typename Scorer>
struct max_scored_cursor {
    using enum_type = typename Index::document_enumerator;
    enum_type docs_enum;
    float q_weight;
    Scorer scorer;
    float max_weight;
};

template <typename Index, typename WandType, typename Scorer>
[[nodiscard]] auto make_max_scored_cursors(Index const &index,
                                           WandType const &wdata,
                                           Scorer const &scorer,
                                           Query query)
{
    using term_scorer_type = std::decay_t<decltype(scorer.term_scorer(0))>;
    auto terms = query.terms;
    auto query_term_freqs = query_freqs(terms);

    std::vector<max_scored_cursor<Index, term_scorer_type>> cursors;
    cursors.reserve(query_term_freqs.size());
    std::transform(query_term_freqs.begin(),
                   query_term_freqs.end(),
                   std::back_inserter(cursors),
                   [&](auto &&term) {
                       auto list = index[term.first];
                       float q_weight = term.second;
                       auto max_weight = q_weight * wdata.max_term_weight(term.first);
                       return max_scored_cursor<Index, term_scorer_type>{
                           std::move(list), q_weight, scorer.term_scorer(term.first), max_weight};
                   });
    return cursors;
}

} // namespace pisa
