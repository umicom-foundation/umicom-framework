/* Umicom Framework | Workspace layout library v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_UI_LAYOUT_LIBRARY_H
#define UMICOM_UI_LAYOUT_LIBRARY_H
#include "umicom/ui/workspace_layout.h"
#define UMI_UI_LAYOUT_LIBRARY_MAX 32U
typedef struct UmiUiLayoutLibraryItem { char preset_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char category[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char description[384U]; UmiUiWorkspaceLayout layout; } UmiUiLayoutLibraryItem;
typedef struct UmiUiLayoutLibrary { UmiUiLayoutLibraryItem items[UMI_UI_LAYOUT_LIBRARY_MAX]; size_t count; uint64_t revision; } UmiUiLayoutLibrary;
UmiStatus umi_ui_layout_library_add(UmiUiLayoutLibrary *library,const UmiUiLayoutLibraryItem *item);
const UmiUiLayoutLibraryItem *umi_ui_layout_library_find(const UmiUiLayoutLibrary *library,const char *preset_id);
UmiStatus umi_ui_layout_library_instantiate(const UmiUiLayoutLibrary *library,const char *preset_id,const char *layout_id,const char *name,UmiUiWorkspaceLayout *out_layout);
#endif
