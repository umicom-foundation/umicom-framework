/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/browser.h
 *
 * PURPOSE:
 *   Build Layout Browser result sets from persisted layout metadata with text, owner, application, category, tag and sorting filters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_BROWSER_H
#define UMICOM_WORKBENCH_LAYOUT_BROWSER_H

#include "umicom/workbench_layout/store.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout browser query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutBrowserQuery {
    uint32_t structure_size;
    char text[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    char category[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char required_tag[UMI_WORKBENCH_LAYOUT_TAG_CAPACITY];
    UmiWorkbenchLayoutSortOrder sort_order;
    bool include_built_in;
    bool include_shared;
    bool include_read_only;
    size_t offset;
    size_t limit;
} UmiWorkbenchLayoutBrowserQuery;

/**
 * Represent the workbench layout browser item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutBrowserItem {
    uint32_t structure_size;
    UmiWorkbenchLayoutRecordSummary summary;
    bool owned_by_current_user;
    bool editable;
    bool deletable;
    bool shareable;
    bool active;
} UmiWorkbenchLayoutBrowserItem;

/**
 * Represent the workbench layout browser result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutBrowserResult {
    uint32_t structure_size;
    UmiWorkbenchLayoutBrowserItem
        items[UMI_WORKBENCH_LAYOUT_MAX_BROWSER_RESULTS];
    size_t count;
    size_t total_available;
    bool truncated;
    uint64_t revision;
} UmiWorkbenchLayoutBrowserResult;

/**
 * Provide the workbench layout browser query default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutBrowserQuery
umi_workbench_layout_browser_query_default(void);

/**
 * Provide the workbench layout browser search operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_browser_search(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutBrowserQuery *query,
    const char *current_user_id,
    const char *active_layout_id,
    UmiWorkbenchLayoutBrowserResult *out_result);

/**
 * Find workbench layout browser result while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchLayoutBrowserItem *
umi_workbench_layout_browser_result_at(
    const UmiWorkbenchLayoutBrowserResult *result,
    size_t index);

/**
 * Provide the workbench layout browser sort operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_browser_sort(
    UmiWorkbenchLayoutBrowserResult *result,
    UmiWorkbenchLayoutSortOrder order);

#ifdef __cplusplus
}
#endif

#endif
