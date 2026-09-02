/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/query.c
 *
 * PURPOSE:
 *   Implement the query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/query.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiDiagnosticQueryMatch {
    size_t model_index;
    uint64_t sequence;
    UmiDiagnosticSeverity severity;
} UmiDiagnosticQueryMatch;

/*
 * Provide the match sequence ascending operation used by this module and its client
 * applications.
 */
static int match_sequence_ascending(const void *left, const void *right)
{
    const UmiDiagnosticQueryMatch *a = (const UmiDiagnosticQueryMatch *)left;
    const UmiDiagnosticQueryMatch *b = (const UmiDiagnosticQueryMatch *)right;
    return a->sequence < b->sequence ? -1 : (a->sequence > b->sequence ? 1 : 0);
}

/*
 * Provide the match sequence descending operation used by this module and its client
 * applications.
 */
static int match_sequence_descending(const void *left, const void *right)
{
    return -match_sequence_ascending(left, right);
}

/*
 * Provide the match severity descending operation used by this module and its client
 * applications.
 */
static int match_severity_descending(const void *left, const void *right)
{
    const UmiDiagnosticQueryMatch *a = (const UmiDiagnosticQueryMatch *)left;
    const UmiDiagnosticQueryMatch *b = (const UmiDiagnosticQueryMatch *)right;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->severity != b->severity) return a->severity > b->severity ? -1 : 1;
    return match_sequence_descending(left, right);
}

/*
 * Initialise diagnostic query from caller-provided values so later operations receive a
 * known state.
 */
void umi_diagnostic_query_init(UmiDiagnosticQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    umi_diagnostic_filter_init(&query->filter);
    query->sort = UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING;
    query->limit = UMI_DIAGNOSTIC_QUERY_RESULT_MAX;
}

/*
 * Perform diagnostic query through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_diagnostic_query_execute(const UmiDiagnosticModel *model,
                                       const UmiDiagnosticQuery *query,
                                       UmiDiagnosticQueryResult *out_result)
{
    UmiDiagnosticQueryMatch *matches;
    size_t match_count = 0U;
    size_t model_count;
    size_t index;
    size_t start;
    size_t limit;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || query == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    model_count = umi_diagnostic_model_count(model);
    matches = model_count > 0U
        ? (UmiDiagnosticQueryMatch *)calloc(model_count, sizeof(*matches))
        : NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model_count > 0U && matches == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model_count; ++index) {
        UmiDiagnosticSnapshot snapshot;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_diagnostic_model_at(model, index, &snapshot) == UMI_STATUS_OK &&
            umi_diagnostic_filter_matches(&query->filter, &snapshot)) {
            matches[match_count].model_index = index;
            matches[match_count].sequence = snapshot.sequence;
            matches[match_count].severity = snapshot.severity;
            ++match_count;
        }
    }
    out_result->total_matches = match_count;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match_count > 1U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (query->sort == UMI_DIAGNOSTIC_SORT_SEQUENCE_DESCENDING) {
            qsort(matches, match_count, sizeof(*matches), match_sequence_descending);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (query->sort == UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING) {
            qsort(matches, match_count, sizeof(*matches), match_severity_descending);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            qsort(matches, match_count, sizeof(*matches), match_sequence_ascending);
        }
    }
    start = query->offset < match_count ? query->offset : match_count;
    limit = query->limit == 0U ? UMI_DIAGNOSTIC_QUERY_RESULT_MAX : query->limit;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit > UMI_DIAGNOSTIC_QUERY_RESULT_MAX) limit = UMI_DIAGNOSTIC_QUERY_RESULT_MAX;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (start < match_count && out_result->count < limit) {
        UmiStatus status = umi_diagnostic_model_at(
            model, matches[start++].model_index,
            &out_result->items[out_result->count]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            free(matches);
            return status;
        }
        ++out_result->count;
    }
    out_result->truncated = start < match_count;
    free(matches);
    return UMI_STATUS_OK;
}
