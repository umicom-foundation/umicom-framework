/* Umicom Framework | Workspace layout library v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_UI_LAYOUT_LIBRARY_V2_H
#define UMICOM_UI_LAYOUT_LIBRARY_V2_H
#include "umicom/ui/workspace_layout_v3.h"
#define UMI_UI_LAYOUT_LIBRARY_MAX 32U
typedef struct UmiUiLayoutLibraryItemV2 { char preset_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char category[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char description[384U]; UmiUiWorkspaceLayoutV3 layout; } UmiUiLayoutLibraryItemV2;
typedef struct UmiUiLayoutLibraryV2 { UmiUiLayoutLibraryItemV2 items[UMI_UI_LAYOUT_LIBRARY_MAX]; size_t count; uint64_t revision; } UmiUiLayoutLibraryV2;
UmiStatus umi_ui_layout_library_v2_add(UmiUiLayoutLibraryV2 *library,const UmiUiLayoutLibraryItemV2 *item);
const UmiUiLayoutLibraryItemV2 *umi_ui_layout_library_v2_find(const UmiUiLayoutLibraryV2 *library,const char *preset_id);
UmiStatus umi_ui_layout_library_v2_instantiate(const UmiUiLayoutLibraryV2 *library,const char *preset_id,const char *layout_id,const char *name,UmiUiWorkspaceLayoutV3 *out_layout);
#endif
