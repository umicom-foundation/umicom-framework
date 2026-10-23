/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/explorer_page.c
 * PURPOSE: Keep page bounds and copied test rows independent of native widget lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/explorer_page.h"
#include <stdlib.h>
#include <string.h>
struct UmiTestExplorerPage {
    const UmiTestExplorerModel *owner;
    UmiTestExplorerPageSummary summary;
    UmiTestExplorerRow *rows;
};
UmiStatus UmiTestExplorerPageCreate(const UmiTestExplorerModel *model,
    size_t offset, size_t page_size, UmiTestExplorerPage **out_page)
{
    if (out_page == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_page = NULL;
    if (model == NULL || page_size == 0U || page_size > UMI_TEST_EXPLORER_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestExplorerPage *page = calloc(1U, sizeof *page);
    if (page == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_test_explorer_model_snapshot(model, &page->summary.model);
    if (status != UMI_STATUS_OK) { free(page); return status; }
    UmiTestExplorerPageSummary *s = &page->summary;
    size_t total = s->model.visible_row_count;
    s->page_size = page_size;
    /* Subtraction before division avoids overflow for even SIZE_MAX offsets. */
    if (total != 0U) {
        s->page_count = 1U + (total - 1U) / page_size;
        size_t last = (s->page_count - 1U) * page_size;
        s->offset = offset > last ? last : (offset / page_size) * page_size;
        s->page_index = s->offset / page_size;
        s->count = total - s->offset;
        if (s->count > page_size) s->count = page_size;
        s->has_previous = s->offset != 0U;
        s->has_next = s->count < total - s->offset;
        s->previous_offset = s->has_previous ? s->offset - page_size : s->offset;
        s->next_offset = s->has_next ? s->offset + s->count : s->offset;
    }
    if (s->count != 0U) {
        page->rows = calloc(s->count, sizeof *page->rows);
        if (page->rows == NULL) { free(page); return UMI_STATUS_OUT_OF_MEMORY; }
    }
    page->owner = model;
    for (size_t i = 0U; i < s->count; ++i) {
        status = umi_test_explorer_model_visible_row_at(model, s->offset + i, &page->rows[i]);
        if (status != UMI_STATUS_OK) { UmiTestExplorerPageDestroy(page); return status; }
    }
    status = UmiTestExplorerPageValidate(page, model);
    if (status != UMI_STATUS_OK) { UmiTestExplorerPageDestroy(page); return status; }
    *out_page = page;
    return UMI_STATUS_OK;
}
void UmiTestExplorerPageDestroy(UmiTestExplorerPage *page)
{
    if (page != NULL) { free(page->rows); free(page); }
}
UmiStatus UmiTestExplorerPageRead(const UmiTestExplorerPage *page,
    UmiTestExplorerPageSummary *out_summary)
{
    if (page == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = page->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestExplorerPageRowAt(const UmiTestExplorerPage *page,
    size_t position, UmiTestExplorerRow *out_row)
{
    if (page == NULL || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= page->summary.count) return UMI_STATUS_NOT_FOUND;
    *out_row = page->rows[position];
    return UMI_STATUS_OK;
}
UmiStatus UmiTestExplorerPageValidate(const UmiTestExplorerPage *page,
    const UmiTestExplorerModel *model)
{
    if (page == NULL || model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (page->owner != model || page->summary.model.revision != umi_test_explorer_model_revision(model))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
