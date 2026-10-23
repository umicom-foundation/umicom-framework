/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostic_ui/page.c
 * PURPOSE: Centralise revision-checked diagnostic paging and severity totals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostic_ui/page.h"
#include <stdlib.h>
#include <string.h>
struct UmiDiagnosticPage {
    const UmiDiagnosticModel *model;
    UmiDiagnosticQuery query;
    UmiDiagnosticPageSummary summary;
    UmiDiagnosticSnapshot *items;
};
static UmiStatus PageQuery(const UmiDiagnosticQuery *input, UmiDiagnosticQuery *output)
{
    if (input == NULL || output == NULL ||
        memchr(input->filter.source, 0, sizeof input->filter.source) == NULL ||
        memchr(input->filter.owner, 0, sizeof input->filter.owner) == NULL ||
        memchr(input->filter.text, 0, sizeof input->filter.text) == NULL ||
        (input->sort != UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING &&
         input->sort != UMI_DIAGNOSTIC_SORT_SEQUENCE_DESCENDING &&
         input->sort != UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (input->limit > UMI_DIAGNOSTIC_PAGE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    *output = *input;
    output->limit = input->limit == 0U ? UMI_DIAGNOSTIC_PAGE_LIMIT : input->limit;
    output->filter.include_resolved = input->filter.include_resolved != 0;
    output->filter.include_transient = input->filter.include_transient != 0;
    return UMI_STATUS_OK;
}
static int PageQueriesEqual(const UmiDiagnosticQuery *a, const UmiDiagnosticQuery *b)
{
    return a->sort == b->sort && a->offset == b->offset && a->limit == b->limit &&
        a->filter.severity_mask == b->filter.severity_mask && a->filter.kind_mask == b->filter.kind_mask &&
        a->filter.include_resolved == b->filter.include_resolved &&
        a->filter.include_transient == b->filter.include_transient &&
        strcmp(a->filter.source, b->filter.source) == 0 &&
        strcmp(a->filter.owner, b->filter.owner) == 0 && strcmp(a->filter.text, b->filter.text) == 0;
}
void UmiDiagnosticPageDestroy(UmiDiagnosticPage *page)
{
    if (page == NULL) return;
    free(page->items);
    free(page);
}
UmiStatus UmiDiagnosticPageCreate(const UmiDiagnosticModel *model,
    const UmiDiagnosticQuery *query, UmiDiagnosticPage **out_page)
{
    UmiDiagnosticQuery checked;
    if (model == NULL || out_page == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PageQuery(query, &checked);
    if (status != UMI_STATUS_OK) return status;
    UmiDiagnosticPage *page = calloc(1U, sizeof *page);
    UmiDiagnosticQueryResult *part = calloc(1U, sizeof *part);
    if (page == NULL || part == NULL) { free(page); free(part); return UMI_STATUS_OUT_OF_MEMORY; }
    page->items = calloc(checked.limit, sizeof *page->items);
    if (page->items == NULL) { free(part); UmiDiagnosticPageDestroy(page); return UMI_STATUS_OUT_OF_MEMORY; }
    page->model = model;
    page->query = checked;
    page->summary.revision = umi_diagnostic_model_revision(model);
    /* Totals describe the entire retained model, regardless of the current
     * filter. Severity totals exclude resolved findings, matching the pane. */
    size_t retained = umi_diagnostic_model_count(model);
    for (size_t i = 0U; i < retained; ++i) {
        UmiDiagnosticSnapshot item;
        status = umi_diagnostic_model_at(model, i, &item);
        if (status != UMI_STATUS_OK) goto done;
        if (item.resolved) { ++page->summary.resolved; continue; }
        ++page->summary.active;
        if (item.severity == UMI_DIAGNOSTIC_WARNING) ++page->summary.warnings;
        if (item.severity == UMI_DIAGNOSTIC_ERROR) ++page->summary.errors;
        if (item.severity == UMI_DIAGNOSTIC_FATAL) ++page->summary.fatal;
    }
    for (;;) {
        UmiDiagnosticQuery next = checked;
        next.offset = checked.offset + page->summary.count;
        next.limit = checked.limit - page->summary.count;
        if (next.limit > UMI_DIAGNOSTIC_QUERY_RESULT_MAX) next.limit = UMI_DIAGNOSTIC_QUERY_RESULT_MAX;
        status = umi_diagnostic_query_execute(model, &next, part);
        if (status != UMI_STATUS_OK) goto done;
        page->summary.total_matches = part->total_matches;
        memcpy(page->items + page->summary.count, part->items, part->count * sizeof *part->items);
        page->summary.count += part->count;
        page->summary.has_more = part->truncated;
        if (!part->truncated || part->count == 0U || page->summary.count == checked.limit) break;
        /* Offsets beyond the model yield no rows, so large caller offsets do
         * not overflow this addition. Still reject an inconsistent provider. */
        if (checked.offset > SIZE_MAX - page->summary.count) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto done; }
    }
    if (umi_diagnostic_model_revision(model) != page->summary.revision) status = UMI_STATUS_INVALID_STATE;
 done:
    free(part);
    if (status != UMI_STATUS_OK) { UmiDiagnosticPageDestroy(page); return status; }
    *out_page = page;
    return UMI_STATUS_OK;
}
UmiStatus UmiDiagnosticPageReadSummary(const UmiDiagnosticPage *page,
    UmiDiagnosticPageSummary *out_summary)
{
    if (page == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = page->summary;
    return UMI_STATUS_OK;
}
const UmiDiagnosticSnapshot *UmiDiagnosticPageItem(const UmiDiagnosticPage *page, size_t index)
{
    return page != NULL && index < page->summary.count ? &page->items[index] : NULL;
}
UmiStatus UmiDiagnosticPageValidate(const UmiDiagnosticPage *page,
    const UmiDiagnosticModel *model, const UmiDiagnosticQuery *query)
{
    UmiDiagnosticQuery checked;
    if (page == NULL || model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PageQuery(query, &checked);
    if (status != UMI_STATUS_OK) return status;
    return page->model == model && page->summary.revision == umi_diagnostic_model_revision(model) &&
        PageQueriesEqual(&page->query, &checked) ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
