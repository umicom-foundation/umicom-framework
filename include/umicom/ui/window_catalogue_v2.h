/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/window_catalogue_v2.h
 *
 * PURPOSE:
 *   Publish the public window catalogue v2 contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WINDOW_CATALOGUE_V2_H
#define UMICOM_UI_WINDOW_CATALOGUE_V2_H
#include "umicom/ui/workspace_layout_v3.h"
#define UMI_UI_WINDOW_CATALOGUE_MAX 256U
typedef enum UmiUiWindowCategory { UMI_UI_WINDOW_CATEGORY_DEVELOPMENT = 1, UMI_UI_WINDOW_CATEGORY_NAVIGATION, UMI_UI_WINDOW_CATEGORY_OPERATIONS, UMI_UI_WINDOW_CATEGORY_DATA, UMI_UI_WINDOW_CATEGORY_AI, UMI_UI_WINDOW_CATEGORY_TRADING, UMI_UI_WINDOW_CATEGORY_GENERAL } UmiUiWindowCategory;
typedef struct UmiUiWindowDescriptor { char tool_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char title[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY]; char description[384U]; char icon_name[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; UmiUiWindowCategory category; bool supports_multiple; double default_width; double default_height; } UmiUiWindowDescriptor;
typedef struct UmiUiWindowCatalogueV2 { UmiUiWindowDescriptor items[UMI_UI_WINDOW_CATALOGUE_MAX]; size_t count; uint64_t revision; } UmiUiWindowCatalogueV2;
UmiStatus umi_ui_window_catalogue_v2_register(UmiUiWindowCatalogueV2 *catalogue,const UmiUiWindowDescriptor *descriptor);
const UmiUiWindowDescriptor *umi_ui_window_catalogue_v2_find(const UmiUiWindowCatalogueV2 *catalogue,const char *tool_id);
size_t umi_ui_window_catalogue_v2_search(const UmiUiWindowCatalogueV2 *catalogue,const char *query,UmiUiWindowCategory category,const UmiUiWindowDescriptor **out_items,size_t capacity);
#endif
