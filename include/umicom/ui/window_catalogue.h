/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/window_catalogue.h
 *
 * PURPOSE:
 *   Publish the public window catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WINDOW_CATALOGUE_H
#define UMICOM_UI_WINDOW_CATALOGUE_H
#include "umicom/ui/workspace_layout.h"
#define UMI_UI_WINDOW_CATALOGUE_MAX 256U
#define UMI_UI_WINDOW_RECENT_MAX 32U
/**
 * List the named ui window category values accepted by this public contract.
 */
typedef enum UmiUiWindowCategory { UMI_UI_WINDOW_CATEGORY_DEVELOPMENT = 1, UMI_UI_WINDOW_CATEGORY_NAVIGATION, UMI_UI_WINDOW_CATEGORY_OPERATIONS, UMI_UI_WINDOW_CATEGORY_DATA, UMI_UI_WINDOW_CATEGORY_AI, UMI_UI_WINDOW_CATEGORY_TRADING, UMI_UI_WINDOW_CATEGORY_GENERAL } UmiUiWindowCategory;
/**
 * List the named ui window catalogue order values accepted by this public contract.
 */
typedef enum UmiUiWindowCatalogueOrder {
    UMI_UI_WINDOW_CATALOGUE_ORDER_RECOMMENDED = 1,
    UMI_UI_WINDOW_CATALOGUE_ORDER_ALPHABETICAL = 2,
    UMI_UI_WINDOW_CATALOGUE_ORDER_RECENT = 3
} UmiUiWindowCatalogueOrder;
/**
 * Represent the ui window descriptor data shared with callers of this public contract.
 */
typedef struct UmiUiWindowDescriptor { char tool_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char title[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY]; char description[384U]; char icon_name[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; UmiUiWindowCategory category; bool supports_multiple; double default_width; double default_height; } UmiUiWindowDescriptor;
/**
 * Represent the ui window recent entry data shared with callers of this public contract.
 */
typedef struct UmiUiWindowRecentEntry {
    char tool_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    uint64_t last_opened_at_ms;
    uint64_t open_count;
} UmiUiWindowRecentEntry;
/**
 * Represent the ui window catalogue data shared with callers of this public contract.
 */
typedef struct UmiUiWindowCatalogue {
    UmiUiWindowDescriptor items[UMI_UI_WINDOW_CATALOGUE_MAX];
    size_t count;
    UmiUiWindowRecentEntry recent[UMI_UI_WINDOW_RECENT_MAX];
    size_t recent_count;
    uint64_t revision;
} UmiUiWindowCatalogue;
/**
 * Represent the ui window catalogue query data shared with callers of this public
 * contract.
 */
typedef struct UmiUiWindowCatalogueQuery {
    char text[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    UmiUiWindowCategory category;
    UmiUiWindowCatalogueOrder order;
    uint32_t category_mask;
    bool include_singletons_already_open;
    size_t limit;
} UmiUiWindowCatalogueQuery;
/**
 * Represent the ui window catalogue item data shared with callers of this public contract.
 */
typedef struct UmiUiWindowCatalogueItem {
    const UmiUiWindowDescriptor *descriptor;
    size_t open_instance_count;
    uint64_t last_opened_at_ms;
    uint64_t open_count;
    bool can_open;
    bool recent;
} UmiUiWindowCatalogueItem;
/**
 * Represent the ui window catalogue result data shared with callers of this public
 * contract.
 */
typedef struct UmiUiWindowCatalogueResult {
    UmiUiWindowCatalogueItem items[UMI_UI_WINDOW_CATALOGUE_MAX];
    size_t count;
    size_t total_available;
    bool truncated;
    uint64_t revision;
} UmiUiWindowCatalogueResult;

/**
 * Initialise ui window catalogue from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_window_catalogue_init(UmiUiWindowCatalogue *catalogue);
/**
 * Add ui window catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_window_catalogue_register(UmiUiWindowCatalogue *catalogue,const UmiUiWindowDescriptor *descriptor);
/**
 * Find ui window catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiWindowDescriptor *umi_ui_window_catalogue_find(const UmiUiWindowCatalogue *catalogue,const char *tool_id);
/**
 * Provide the ui window catalogue search operation used by this module and its client
 * applications.
 */
size_t umi_ui_window_catalogue_search(const UmiUiWindowCatalogue *catalogue,const char *query,UmiUiWindowCategory category,const UmiUiWindowDescriptor **out_items,size_t capacity);
/**
 * Provide the ui window catalogue query default operation used by this module and its
 * client applications.
 */
UmiUiWindowCatalogueQuery umi_ui_window_catalogue_query_default(void);
/**
 * Provide the ui window catalogue query operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_window_catalogue_query(
    const UmiUiWindowCatalogue *catalogue,
    const UmiUiWorkspaceLayout *active_layout,
    const UmiUiWindowCatalogueQuery *query,
    UmiUiWindowCatalogueResult *out_result);
/**
 * Provide the ui window catalogue record open operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_window_catalogue_record_open(
    UmiUiWindowCatalogue *catalogue,
    const char *tool_id,
    uint64_t opened_at_ms);
/**
 * Provide the ui window category text operation used by this module and its client
 * applications.
 */
const char *umi_ui_window_category_text(UmiUiWindowCategory category);
#endif
