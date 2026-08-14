/* Umicom Framework | Workspace customisation centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_UI_WORKSPACE_CUSTOMISATION_V2_H
#define UMICOM_UI_WORKSPACE_CUSTOMISATION_V2_H
#include "umicom/ui/window_catalogue_v2.h"
#include "umicom/ui/window_group_v2.h"
#include "umicom/ui/layout_library_v2.h"
#include "umicom/ui/theme_profile_v2.h"
#define UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS 16U
typedef struct UmiUiWorkspaceCustomisationV2 { UmiUiWorkspaceLayoutV3 layouts[UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS]; size_t layout_count; char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; UmiUiWindowCatalogueV2 windows; UmiUiWindowGroupStoreV2 groups; UmiUiLayoutLibraryV2 library; UmiUiThemeProfileV2 theme; uint64_t revision; } UmiUiWorkspaceCustomisationV2;
typedef struct UmiUiWorkspaceCustomisationSnapshotV2 { size_t layouts; size_t available_windows; size_t groups; size_t presets; char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char theme_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; bool active_layout_locked; uint64_t revision; } UmiUiWorkspaceCustomisationSnapshotV2;
void umi_ui_workspace_customisation_v2_init(UmiUiWorkspaceCustomisationV2 *customisation);
UmiStatus umi_ui_workspace_customisation_v2_add_layout(UmiUiWorkspaceCustomisationV2 *customisation,const UmiUiWorkspaceLayoutV3 *layout);
UmiStatus umi_ui_workspace_customisation_v2_activate(UmiUiWorkspaceCustomisationV2 *customisation,const char *layout_id);
UmiUiWorkspaceLayoutV3 *umi_ui_workspace_customisation_v2_active(UmiUiWorkspaceCustomisationV2 *customisation);
UmiStatus umi_ui_workspace_customisation_v2_set_theme(UmiUiWorkspaceCustomisationV2 *customisation,const UmiUiThemeProfileV2 *theme);
void umi_ui_workspace_customisation_v2_snapshot(const UmiUiWorkspaceCustomisationV2 *customisation,UmiUiWorkspaceCustomisationSnapshotV2 *out_snapshot);
#endif
