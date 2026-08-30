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
typedef enum UmiUiWindowCategory { UMI_UI_WINDOW_CATEGORY_DEVELOPMENT = 1, UMI_UI_WINDOW_CATEGORY_NAVIGATION, UMI_UI_WINDOW_CATEGORY_OPERATIONS, UMI_UI_WINDOW_CATEGORY_DATA, UMI_UI_WINDOW_CATEGORY_AI, UMI_UI_WINDOW_CATEGORY_TRADING, UMI_UI_WINDOW_CATEGORY_GENERAL } UmiUiWindowCategory;
typedef struct UmiUiWindowDescriptor { char tool_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char title[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY]; char description[384U]; char icon_name[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; UmiUiWindowCategory category; bool supports_multiple; double default_width; double default_height; } UmiUiWindowDescriptor;
typedef struct UmiUiWindowCatalogue { UmiUiWindowDescriptor items[UMI_UI_WINDOW_CATALOGUE_MAX]; size_t count; uint64_t revision; } UmiUiWindowCatalogue;
UmiStatus umi_ui_window_catalogue_register(UmiUiWindowCatalogue *catalogue,const UmiUiWindowDescriptor *descriptor);
const UmiUiWindowDescriptor *umi_ui_window_catalogue_find(const UmiUiWindowCatalogue *catalogue,const char *tool_id);
size_t umi_ui_window_catalogue_search(const UmiUiWindowCatalogue *catalogue,const char *query,UmiUiWindowCategory category,const UmiUiWindowDescriptor **out_items,size_t capacity);
#endif
