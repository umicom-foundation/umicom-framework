/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/completion_query.c
 *
 * PURPOSE:
 *   Implement bounded case-aware prefix, substring and fuzzy completion
 *   matching with deterministic scoring, provider de-duplication and ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/completion_query.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorCompletionQueryResult {
    UmiEditorCompletionMatch *matches;
    size_t count;
    size_t capacity;
    size_t source_candidate_count;
    size_t matched_candidate_count;
    size_t filtered_candidate_count;
    size_t deduplicated_candidate_count;
    uint64_t source_revision;
    uint64_t request_id;
    uint64_t document_revision;
    uint64_t revision;
    int truncated;
    int cancelled;
};

typedef struct TextMatchMetrics {
    int matched;
    int exact;
    int prefix;
    int32_t score;
    size_t matched_count;
    size_t boundary_count;
    size_t longest_run;
} TextMatchMetrics;

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the clamp score operation used by this module and its client applications. */
static int32_t clamp_score(int64_t score)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (score > INT32_MAX) return INT32_MAX;
    /* Apply this branch only when its contract condition is satisfied. */
    if (score < INT32_MIN) return INT32_MIN;
    return (int32_t)score;
}

/* Provide the folded operation used by this module and its client applications. */
static unsigned char folded(unsigned char value, int case_sensitive)
{
    return case_sensitive
        ? value
        : (unsigned char)tolower((int)value);
}

/* Provide the same character operation used by this module and its client applications. */
static int same_character(unsigned char left,
                          unsigned char right,
                          int case_sensitive)
{
    return folded(left, case_sensitive) == folded(right, case_sensitive);
}

/*
 * Provide the prefix contains uppercase operation used by this module and its client
 * applications.
 */
static int prefix_contains_uppercase(const char *prefix)
{
    const unsigned char *cursor = (const unsigned char *)prefix;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (isupper((int)*cursor) != 0) return 1;
        ++cursor;
    }
    return 0;
}

/* Provide the is word boundary operation used by this module and its client applications. */
static int is_word_boundary(const char *text, size_t position)
{
    const unsigned char current = (unsigned char)text[position];
    unsigned char previous;

    /* Apply this branch only when its contract condition is satisfied. */
    if (position == 0U) return 1;
    previous = (unsigned char)text[position - 1U];
    /* Apply this branch only when its contract condition is satisfied. */
    if (isalnum((int)previous) == 0 && previous != (unsigned char)'_') {
        return 1;
    }
    return islower((int)previous) != 0 && isupper((int)current) != 0;
}

/* Provide the find substring operation used by this module and its client applications. */
static size_t find_substring(const char *text,
                             const char *query,
                             int case_sensitive)
{
    const size_t text_length = strlen(text);
    const size_t query_length = strlen(query);
    size_t start;
    size_t offset;

    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length == 0U) return 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length > text_length) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (start = 0U; start + query_length <= text_length; ++start) {
        int matches = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < query_length; ++offset) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (!same_character((unsigned char)text[start + offset],
                                (unsigned char)query[offset],
                                case_sensitive)) {
                matches = 0;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matches) return start;
    }
    return SIZE_MAX;
}

/* Provide the match text operation used by this module and its client applications. */
static TextMatchMetrics match_text(const char *text,
                                   const char *query,
                                   UmiEditorCompletionMatchMode mode,
                                   int case_sensitive,
                                   const UmiEditorCompletionRankingConfig *config)
{
    TextMatchMetrics metrics;
    const size_t text_length = strlen(text);
    const size_t query_length = strlen(query);
    size_t offset;

    (void)memset(&metrics, 0, sizeof(metrics));
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length == 0U) {
        metrics.matched = 1;
        return metrics;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length == text_length) {
        int equal = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < query_length; ++offset) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (!same_character((unsigned char)text[offset],
                                (unsigned char)query[offset],
                                case_sensitive)) {
                equal = 0;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (equal) {
            metrics.matched = 1;
            metrics.exact = 1;
            metrics.prefix = 1;
            metrics.matched_count = query_length;
            metrics.longest_run = query_length;
            metrics.boundary_count = query_length > 0U ? 1U : 0U;
            metrics.score = config->exact_match_bonus;
            return metrics;
        }
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (mode == UMI_EDITOR_COMPLETION_MATCH_PREFIX ||
        mode == UMI_EDITOR_COMPLETION_MATCH_SUBSTRING ||
        mode == UMI_EDITOR_COMPLETION_MATCH_FUZZY) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (query_length <= text_length) {
            int prefix = 1;
            /* Visit each bounded item once so every record receives the same rule. */
            for (offset = 0U; offset < query_length; ++offset) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (!same_character((unsigned char)text[offset],
                                    (unsigned char)query[offset],
                                    case_sensitive)) {
                    prefix = 0;
                    break;
                }
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (prefix) {
                metrics.matched = 1;
                metrics.prefix = 1;
                metrics.matched_count = query_length;
                metrics.longest_run = query_length;
                metrics.boundary_count = query_length > 0U ? 1U : 0U;
                metrics.score = config->prefix_match_bonus;
                return metrics;
            }
        }
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (mode == UMI_EDITOR_COMPLETION_MATCH_PREFIX) return metrics;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (mode == UMI_EDITOR_COMPLETION_MATCH_SUBSTRING) {
        const size_t position = find_substring(text, query, case_sensitive);
        /* Apply this branch only when its contract condition is satisfied. */
        if (position != SIZE_MAX) {
            const int64_t penalty = position > (size_t)INT32_MAX
                ? INT32_MAX
                : (int64_t)(int32_t)position;
            metrics.matched = 1;
            metrics.matched_count = query_length;
            metrics.longest_run = query_length;
            metrics.boundary_count = is_word_boundary(text, position) ? 1U : 0U;
            metrics.score = clamp_score(600 - penalty +
                (int64_t)metrics.boundary_count * config->boundary_bonus);
        }
        return metrics;
    }
    {
        size_t query_position = 0U;
        size_t run = 0U;
        size_t gap_count = 0U;
        size_t previous_match = SIZE_MAX;
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U;
             offset < text_length && query_position < query_length;
             ++offset) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (same_character((unsigned char)text[offset],
                               (unsigned char)query[query_position],
                               case_sensitive)) {
                ++metrics.matched_count;
                /* Apply this branch only when its contract condition is satisfied. */
                if (is_word_boundary(text, offset)) ++metrics.boundary_count;
                /* Use the stable identifier comparison to choose the matching record or policy. */
                if (previous_match != SIZE_MAX && offset == previous_match + 1U) {
                    ++run;
                } /* Use this fallback path when the earlier condition does not apply. */ else {
                    run = 1U;
                }
                /* Apply this branch only when its contract condition is satisfied. */
                if (run > metrics.longest_run) metrics.longest_run = run;
                previous_match = offset;
                ++query_position;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (query_position > 0U) {
                ++gap_count;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (query_position == query_length) {
            const int64_t matched_bonus =
                query_length > (size_t)INT32_MAX
                    ? INT32_MAX
                    : (int64_t)(int32_t)query_length * 10;
            const int64_t boundary_bonus =
                metrics.boundary_count > (size_t)INT32_MAX
                    ? INT32_MAX
                    : (int64_t)(int32_t)metrics.boundary_count *
                          config->boundary_bonus;
            const int64_t consecutive_bonus =
                metrics.longest_run > (size_t)INT32_MAX
                    ? INT32_MAX
                    : (int64_t)(int32_t)metrics.longest_run *
                          config->consecutive_bonus;
            const int64_t gap_penalty =
                gap_count > (size_t)INT32_MAX
                    ? INT32_MAX
                    : (int64_t)(int32_t)gap_count * 2;
            metrics.matched = 1;
            metrics.score = clamp_score(250 + matched_bonus + boundary_bonus +
                                        consecutive_bonus - gap_penalty);
        }
    }
    return metrics;
}

/* Provide the validate config operation used by this module and its client applications. */
static UmiStatus validate_config(
    const UmiEditorCompletionRankingConfig *config)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL ||
        config->struct_size != (uint32_t)sizeof(*config) ||
        config->api_version != UMI_EDITOR_COMPLETION_QUERY_API_VERSION ||
        config->match_mode < UMI_EDITOR_COMPLETION_MATCH_PREFIX ||
        config->match_mode > UMI_EDITOR_COMPLETION_MATCH_FUZZY ||
        config->maximum_results == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve matches operation used by this module and its client applications. */
static UmiStatus reserve_matches(UmiEditorCompletionQueryResult *result,
                                 size_t required)
{
    size_t capacity;
    UmiEditorCompletionMatch *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= result->capacity) return UMI_STATUS_OK;
    capacity = result->capacity > 0U ? result->capacity : 32U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorCompletionMatch *)realloc(
        result->matches, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    result->matches = replacement;
    result->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the match compare operation used by this module and its client applications. */
static int match_compare(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorCompletionMatch *left =
        (const UmiEditorCompletionMatch *)left_pointer;
    const UmiEditorCompletionMatch *right =
        (const UmiEditorCompletionMatch *)right_pointer;
    const char *left_sort = left->candidate.item.sort_text[0] != '\0'
        ? left->candidate.item.sort_text
        : left->candidate.item.label;
    const char *right_sort = right->candidate.item.sort_text[0] != '\0'
        ? right->candidate.item.sort_text
        : right->candidate.item.label;
    int order;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->final_score > right->final_score) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->final_score < right->final_score) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->candidate.provider_priority >
        right->candidate.provider_priority) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->candidate.provider_priority <
        right->candidate.provider_priority) return 1;
    order = strcmp(left_sort, right_sort);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    order = strcmp(left->candidate.item.label, right->candidate.item.label);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    order = strcmp(left->candidate.provider_id, right->candidate.provider_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    order = strcmp(left->candidate.item.id, right->candidate.item.id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->source_position < right->source_position) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->source_position > right->source_position) return 1;
    return 0;
}

/* Provide the candidate allowed operation used by this module and its client applications. */
static int candidate_allowed(const UmiEditorCompletionCandidate *candidate,
                             const UmiEditorCompletionRequest *request)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (candidate->item.document_id[0] != '\0' &&
        strcmp(candidate->item.document_id, request->document_id) != 0) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!request->allow_deprecated &&
        ((candidate->flags & UMI_EDITOR_COMPLETION_CANDIDATE_DEPRECATED) != 0U ||
         candidate->item.deprecated)) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!request->allow_snippets &&
        candidate->insert_format == UMI_EDITOR_COMPLETION_INSERT_SNIPPET) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!request->allow_ai &&
        (candidate->flags & UMI_EDITOR_COMPLETION_CANDIDATE_AI) != 0U) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!request->allow_remote &&
        (candidate->flags & UMI_EDITOR_COMPLETION_CANDIDATE_REMOTE) != 0U) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!request->trusted_workspace &&
        (candidate->flags &
         UMI_EDITOR_COMPLETION_CANDIDATE_REQUIRES_TRUST) != 0U) {
        return 0;
    }
    return 1;
}

/* Provide the weighted score operation used by this module and its client applications. */
static int64_t weighted_score(
    const UmiEditorCompletionCandidate *candidate,
    const TextMatchMetrics *metrics,
    const UmiEditorCompletionRankingConfig *config)
{
    int64_t score = metrics->score;

    score += (int64_t)candidate->provider_priority *
             config->provider_priority_weight;
    score += (int64_t)candidate->locality_score * config->locality_weight;
    score += (int64_t)candidate->usage_score * config->usage_weight;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((candidate->flags &
         UMI_EDITOR_COMPLETION_CANDIDATE_PRESELECTED) != 0U) {
        score += config->preselected_bonus;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((candidate->flags &
         UMI_EDITOR_COMPLETION_CANDIDATE_DEPRECATED) != 0U ||
        candidate->item.deprecated) {
        score -= config->deprecated_penalty;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((candidate->flags & UMI_EDITOR_COMPLETION_CANDIDATE_AI) != 0U) {
        score -= config->ai_penalty;
    }
    return score;
}

/* Provide the find duplicate operation used by this module and its client applications. */
static size_t find_duplicate(const UmiEditorCompletionQueryResult *result,
                             const UmiEditorCompletionCandidate *candidate)
{
    size_t position;

    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < result->count; ++position) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_completion_candidate_same_edit(
                &result->matches[position].candidate, candidate)) {
            return position;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the editor completion ranking config default operation used by this module and
 * its client applications.
 */
UmiEditorCompletionRankingConfig umi_editor_completion_ranking_config_default(
    void)
{
    UmiEditorCompletionRankingConfig config;

    (void)memset(&config, 0, sizeof(config));
    config.struct_size = (uint32_t)sizeof(config);
    config.api_version = UMI_EDITOR_COMPLETION_QUERY_API_VERSION;
    config.match_mode = UMI_EDITOR_COMPLETION_MATCH_FUZZY;
    config.maximum_results = 100U;
    config.exact_match_bonus = 1600;
    config.prefix_match_bonus = 1000;
    config.boundary_bonus = 35;
    config.consecutive_bonus = 25;
    config.provider_priority_weight = 20;
    config.locality_weight = 4;
    config.usage_weight = 2;
    config.preselected_bonus = 160;
    config.deprecated_penalty = 400;
    config.ai_penalty = 40;
    config.case_sensitive = 0;
    config.smart_case = 1;
    return config;
}

/*
 * Initialise editor completion query result from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_completion_query_result_create(
    UmiEditorCompletionQueryResult **out_result)
{
    UmiEditorCompletionQueryResult *result;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_result = NULL;
    result = (UmiEditorCompletionQueryResult *)calloc(1U, sizeof(*result));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    result->revision = 1U;
    *out_result = result;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor completion query result so the same storage can be
 * reused safely.
 */
void umi_editor_completion_query_result_destroy(
    UmiEditorCompletionQueryResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return;
    free(result->matches);
    result->matches = NULL;
    free(result);
}

/*
 * Release or reset state held by editor completion query result so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_completion_query_result_clear(
    UmiEditorCompletionQueryResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result->count = 0U;
    result->source_candidate_count = 0U;
    result->matched_candidate_count = 0U;
    result->filtered_candidate_count = 0U;
    result->deduplicated_candidate_count = 0U;
    result->source_revision = 0U;
    result->request_id = 0U;
    result->document_revision = 0U;
    result->truncated = 0;
    result->cancelled = 0;
    result->revision = next_revision(result->revision);
    return UMI_STATUS_OK;
}

/*
 * Perform editor completion query through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_editor_completion_query_execute(
    UmiEditorCompletionQueryResult *result,
    const UmiEditorCompletionCandidateCollection *candidates,
    const UmiEditorCompletionRequest *request,
    const UmiEditorCompletionRankingConfig *config)
{
    size_t source_count;
    int case_sensitive;
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL || candidates == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK ||
        validate_config(config) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    source_count =
        umi_editor_completion_candidate_collection_count(candidates);
    case_sensitive = config->case_sensitive ||
        (config->smart_case && prefix_contains_uppercase(request->prefix));
    status = umi_editor_completion_query_result_clear(result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    result->source_candidate_count = source_count;
    result->source_revision =
        umi_editor_completion_candidate_collection_revision(candidates);
    result->request_id = request->request_id;
    result->document_revision = request->document_revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < source_count; ++position) {
        UmiEditorCompletionCandidate candidate;
        const char *filter_text;
        TextMatchMetrics metrics;
        UmiEditorCompletionMatch match;
        size_t duplicate;
        int64_t score;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_completion_request_is_cancelled(request)) {
            result->cancelled = 1;
            result->revision = next_revision(result->revision);
            return UMI_STATUS_CANCELLED;
        }
        status = umi_editor_completion_candidate_collection_at(
            candidates, position, &candidate);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) continue;
        /* Apply this operation only while the related capability or state is available. */
        if (!candidate_allowed(&candidate, request)) {
            ++result->filtered_candidate_count;
            continue;
        }
        filter_text = candidate.item.filter_text[0] != '\0'
            ? candidate.item.filter_text
            : candidate.item.label;
        metrics = match_text(filter_text,
                             request->prefix,
                             config->match_mode,
                             case_sensitive,
                             config);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!metrics.matched) {
            ++result->filtered_candidate_count;
            continue;
        }
        score = weighted_score(&candidate, &metrics, config);
        candidate.final_score = clamp_score(score);
        (void)memset(&match, 0, sizeof(match));
        match.struct_size = (uint32_t)sizeof(match);
        match.api_version = UMI_EDITOR_COMPLETION_QUERY_API_VERSION;
        match.candidate = candidate;
        match.text_match_score = metrics.score;
        match.final_score = candidate.final_score;
        match.matched_character_count = metrics.matched_count;
        match.boundary_match_count = metrics.boundary_count;
        match.longest_consecutive_run = metrics.longest_run;
        match.source_position = position;
        match.exact_match = metrics.exact;
        match.prefix_match = metrics.prefix;
        ++result->matched_candidate_count;

        duplicate = find_duplicate(result, &candidate);
        /* Apply this branch only when its contract condition is satisfied. */
        if (duplicate != SIZE_MAX) {
            ++result->deduplicated_candidate_count;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (match.final_score > result->matches[duplicate].final_score ||
                (match.final_score == result->matches[duplicate].final_score &&
                 strcmp(match.candidate.provider_id,
                        result->matches[duplicate].candidate.provider_id) < 0)) {
                result->matches[duplicate] = match;
            }
            continue;
        }
        status = reserve_matches(result, result->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        result->matches[result->count++] = match;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (result->count > 1U) {
        qsort(result->matches,
              result->count,
              sizeof(*result->matches),
              match_compare);
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (result->count > config->maximum_results) {
        result->count = config->maximum_results;
        result->truncated = 1;
    }
    result->revision = next_revision(result->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor completion query result while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_completion_query_result_at(
    const UmiEditorCompletionQueryResult *result,
    size_t position,
    UmiEditorCompletionMatch *out_match)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL || out_match == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= result->count) return UMI_STATUS_NOT_FOUND;
    *out_match = result->matches[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor completion query result while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_completion_query_result_find(
    const UmiEditorCompletionQueryResult *result,
    const char *provider_id,
    const char *candidate_id,
    UmiEditorCompletionMatch *out_match)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL || provider_id == NULL || candidate_id == NULL ||
        out_match == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < result->count; ++position) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (strcmp(result->matches[position].candidate.provider_id,
                   provider_id) == 0 &&
            strcmp(result->matches[position].candidate.item.id,
                   candidate_id) == 0) {
            *out_match = result->matches[position];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the editor completion query result snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_query_result_snapshot(
    const UmiEditorCompletionQueryResult *result,
    UmiEditorCompletionQuerySnapshot *out_snapshot)
{
    size_t position;
    size_t comparison;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_COMPLETION_QUERY_API_VERSION;
    out_snapshot->source_candidate_count = result->source_candidate_count;
    out_snapshot->matched_candidate_count = result->matched_candidate_count;
    out_snapshot->filtered_candidate_count = result->filtered_candidate_count;
    out_snapshot->deduplicated_candidate_count =
        result->deduplicated_candidate_count;
    out_snapshot->source_revision = result->source_revision;
    out_snapshot->request_id = result->request_id;
    out_snapshot->document_revision = result->document_revision;
    out_snapshot->revision = result->revision;
    out_snapshot->truncated = result->truncated;
    out_snapshot->cancelled = result->cancelled;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < result->count; ++position) {
        int first_provider = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < position; ++comparison) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (strcmp(result->matches[position].candidate.provider_id,
                       result->matches[comparison].candidate.provider_id) == 0) {
                first_provider = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_provider) ++out_snapshot->provider_count;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((result->matches[position].candidate.flags &
             UMI_EDITOR_COMPLETION_CANDIDATE_AI) != 0U) {
            ++out_snapshot->ai_candidate_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor completion query result without
 * changing their state.
 */
size_t umi_editor_completion_query_result_count(
    const UmiEditorCompletionQueryResult *result)
{
    return result != NULL ? result->count : 0U;
}

/*
 * Provide the editor completion query result revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_completion_query_result_revision(
    const UmiEditorCompletionQueryResult *result)
{
    return result != NULL ? result->revision : 0U;
}
