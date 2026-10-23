/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/explorer_page.h
 * PURPOSE: Capture bounded immutable pages of the canonical Test Explorer projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PLATFORM_EXPLORER_PAGE_H
#define UMICOM_TEST_PLATFORM_EXPLORER_PAGE_H
#include "umicom/test_platform/explorer_model.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TEST_EXPLORER_PAGE_LIMIT 300U
typedef struct UmiTestExplorerPage UmiTestExplorerPage;
typedef struct UmiTestExplorerPageSummary {
    UmiTestExplorerModelSnapshot model;
    size_t offset, count, page_size, page_index, page_count;
    size_t previous_offset, next_offset;
    int has_previous, has_next;
} UmiTestExplorerPageSummary;
/* Capture uses the model's existing order, hierarchy and outcome policy.
 * offset is rounded down to a page boundary and clamped to the last page.
 * Empty projections have zero pages and offset zero. page_size must be 1..300.
 * The owner thread must serialize model changes; this API provides no locking.
 * No files, callbacks, test execution or registry mutations are performed.
 * On failure *out_page is NULL. */
UmiStatus UmiTestExplorerPageCreate(const UmiTestExplorerModel *model,
    size_t offset, size_t page_size, UmiTestExplorerPage **out_page);
void UmiTestExplorerPageDestroy(UmiTestExplorerPage *page);
/* Owned copies remain readable after model destruction. Invalid reads leave
 * caller outputs unchanged. Row positions are relative to the captured page. */
UmiStatus UmiTestExplorerPageRead(const UmiTestExplorerPage *page,
    UmiTestExplorerPageSummary *out_summary);
UmiStatus UmiTestExplorerPageRowAt(const UmiTestExplorerPage *page,
    size_t position, UmiTestExplorerRow *out_row);
/* Validate against the same still-live model. A rebuild, filter, sort,
 * expansion or selection change invalidates this page. Callers must also
 * check source registry revisions when deciding whether a model is current. */
UmiStatus UmiTestExplorerPageValidate(const UmiTestExplorerPage *page,
    const UmiTestExplorerModel *model);
#ifdef __cplusplus
}
#endif
#endif
