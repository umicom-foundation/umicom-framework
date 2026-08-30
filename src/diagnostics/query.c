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

static int match_sequence_ascending(const void *left, const void *right)
{
    const UmiDiagnosticQueryMatch *a = (const UmiDiagnosticQueryMatch *)left;
    const UmiDiagnosticQueryMatch *b = (const UmiDiagnosticQueryMatch *)right;
    return a->sequence < b->sequence ? -1 : (a->sequence > b->sequence ? 1 : 0);
}

static int match_sequence_descending(const void *left, const void *right)
{
    return -match_sequence_ascending(left, right);
}

static int match_severity_descending(const void *left, const void *right)
{
    const UmiDiagnosticQueryMatch *a = (const UmiDiagnosticQueryMatch *)left;
    const UmiDiagnosticQueryMatch *b = (const UmiDiagnosticQueryMatch *)right;
    if (a->severity != b->severity) return a->severity > b->severity ? -1 : 1;
    return match_sequence_descending(left, right);
}

void umi_diagnostic_query_init(UmiDiagnosticQuery *query)
{
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    umi_diagnostic_filter_init(&query->filter);
    query->sort = UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING;
    query->limit = UMI_DIAGNOSTIC_QUERY_RESULT_MAX;
}

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
    if (model == NULL || query == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    model_count = umi_diagnostic_model_count(model);
    matches = model_count > 0U
        ? (UmiDiagnosticQueryMatch *)calloc(model_count, sizeof(*matches))
        : NULL;
    if (model_count > 0U && matches == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (index = 0U; index < model_count; ++index) {
        UmiDiagnosticSnapshot snapshot;
        if (umi_diagnostic_model_at(model, index, &snapshot) == UMI_STATUS_OK &&
            umi_diagnostic_filter_matches(&query->filter, &snapshot)) {
            matches[match_count].model_index = index;
            matches[match_count].sequence = snapshot.sequence;
            matches[match_count].severity = snapshot.severity;
            ++match_count;
        }
    }
    out_result->total_matches = match_count;
    if (match_count > 1U) {
        if (query->sort == UMI_DIAGNOSTIC_SORT_SEQUENCE_DESCENDING) {
            qsort(matches, match_count, sizeof(*matches), match_sequence_descending);
        } else if (query->sort == UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING) {
            qsort(matches, match_count, sizeof(*matches), match_severity_descending);
        } else {
            qsort(matches, match_count, sizeof(*matches), match_sequence_ascending);
        }
    }
    start = query->offset < match_count ? query->offset : match_count;
    limit = query->limit == 0U ? UMI_DIAGNOSTIC_QUERY_RESULT_MAX : query->limit;
    if (limit > UMI_DIAGNOSTIC_QUERY_RESULT_MAX) limit = UMI_DIAGNOSTIC_QUERY_RESULT_MAX;
    while (start < match_count && out_result->count < limit) {
        UmiStatus status = umi_diagnostic_model_at(
            model, matches[start++].model_index,
            &out_result->items[out_result->count]);
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
