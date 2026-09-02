/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/browser.h
 *
 * PURPOSE:
 *   Maintain filtered, sorted and selected Layout Browser entries from
 *   persistent summaries and templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_BROWSER_H
#define UMICOM_WORKBENCH_DESIGNER_BROWSER_H

#include "umicom/workbench_designer/browser_query.h"
#include "umicom/workbench_layout/store.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer browser sort values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerBrowserSort {
    UMI_WORKBENCH_DESIGNER_BROWSER_SORT_NAME = 1,
    UMI_WORKBENCH_DESIGNER_BROWSER_SORT_RECENT = 2,
    UMI_WORKBENCH_DESIGNER_BROWSER_SORT_REVISION = 3,
    UMI_WORKBENCH_DESIGNER_BROWSER_SORT_OWNER = 4
} UmiWorkbenchDesignerBrowserSort;

/**
 * Represent the workbench designer browser item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerBrowserItem {
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char workspace[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char description[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char tags[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t modified_at_ms;
    uint64_t content_hash;
    bool built_in;
    bool locked;
    bool shared;
    bool active;
    bool selected;
} UmiWorkbenchDesignerBrowserItem;

/**
 * Represent the workbench designer browser data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerBrowser {
    UmiWorkbenchDesignerBrowserItem items[UMI_WORKBENCH_DESIGNER_MAX_BROWSER_ITEMS];
    size_t count;
    size_t visible_indices[UMI_WORKBENCH_DESIGNER_MAX_BROWSER_ITEMS];
    size_t visible_count;
    char selected_layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerBrowserQuery query;
    UmiWorkbenchDesignerBrowserSort sort;
    bool descending;
    uint64_t revision;
} UmiWorkbenchDesignerBrowser;

/**
 * Initialise workbench designer browser from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_browser_init(UmiWorkbenchDesignerBrowser *browser);
/**
 * Provide the workbench designer browser add summary operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_browser_add_summary(UmiWorkbenchDesignerBrowser *browser, const UmiWorkbenchLayoutRecordSummary *summary, const char *description, const char *tags, bool active);
/**
 * Provide the workbench designer browser set query operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_browser_set_query(UmiWorkbenchDesignerBrowser *browser, const UmiWorkbenchDesignerBrowserQuery *query);
/**
 * Provide the workbench designer browser set sort operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_browser_set_sort(UmiWorkbenchDesignerBrowser *browser, UmiWorkbenchDesignerBrowserSort sort, bool descending);
/**
 * Provide the workbench designer browser select operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_browser_select(UmiWorkbenchDesignerBrowser *browser, const char *layout_id);
/**
 * Find workbench designer browser visible while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerBrowserItem *umi_workbench_designer_browser_visible_at(const UmiWorkbenchDesignerBrowser *browser, size_t index);
/**
 * Find workbench designer browser while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerBrowserItem *umi_workbench_designer_browser_selected(const UmiWorkbenchDesignerBrowser *browser);

#ifdef __cplusplus
}
#endif

#endif
