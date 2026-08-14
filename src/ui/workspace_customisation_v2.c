/* Umicom Framework | Workspace customisation centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/workspace_customisation_v2.h"
#include <stdio.h>
#include <string.h>

void umi_ui_workspace_customisation_v2_init(UmiUiWorkspaceCustomisationV2 *customisation)
{ if (customisation != NULL) { (void)memset(customisation,0,sizeof(*customisation)); (void)umi_ui_theme_profile_v2_init(&customisation->theme,"umicom-dark","Umicom Dark",UMI_UI_THEME_MODE_DARK,UMI_UI_DENSITY_COMFORTABLE); customisation->revision = 1U; } }
UmiStatus umi_ui_workspace_customisation_v2_add_layout(UmiUiWorkspaceCustomisationV2 *customisation,const UmiUiWorkspaceLayoutV3 *layout)
{
    size_t index;
    if (customisation == NULL || layout == NULL || layout->layout_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < customisation->layout_count; ++index) if (strcmp(customisation->layouts[index].layout_id,layout->layout_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (customisation->layout_count >= UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    customisation->layouts[customisation->layout_count++] = *layout; if (customisation->active_layout_id[0] == '\0') (void)snprintf(customisation->active_layout_id,sizeof(customisation->active_layout_id),"%s",layout->layout_id); customisation->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_customisation_v2_activate(UmiUiWorkspaceCustomisationV2 *customisation,const char *layout_id)
{ size_t index; int length; if (customisation == NULL || layout_id == NULL) return UMI_STATUS_INVALID_ARGUMENT; for (index = 0U; index < customisation->layout_count; ++index) if (strcmp(customisation->layouts[index].layout_id,layout_id) == 0) { length = snprintf(customisation->active_layout_id,sizeof(customisation->active_layout_id),"%s",layout_id); if (length < 0 || (size_t)length >= sizeof(customisation->active_layout_id)) return UMI_STATUS_CAPACITY_EXCEEDED; customisation->revision += 1U; return UMI_STATUS_OK; } return UMI_STATUS_NOT_FOUND; }
UmiUiWorkspaceLayoutV3 *umi_ui_workspace_customisation_v2_active(UmiUiWorkspaceCustomisationV2 *customisation)
{ size_t index; if (customisation == NULL) return NULL; for (index = 0U; index < customisation->layout_count; ++index) if (strcmp(customisation->layouts[index].layout_id,customisation->active_layout_id) == 0) return &customisation->layouts[index]; return NULL; }
UmiStatus umi_ui_workspace_customisation_v2_set_theme(UmiUiWorkspaceCustomisationV2 *customisation,const UmiUiThemeProfileV2 *theme)
{ char reason[192U]; if (customisation == NULL || theme == NULL) return UMI_STATUS_INVALID_ARGUMENT; if (umi_ui_theme_profile_v2_validate(theme,reason,sizeof(reason)) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE; customisation->theme = *theme; customisation->revision += 1U; return UMI_STATUS_OK; }
void umi_ui_workspace_customisation_v2_snapshot(const UmiUiWorkspaceCustomisationV2 *customisation,UmiUiWorkspaceCustomisationSnapshotV2 *out_snapshot)
{
    size_t index;
    if (out_snapshot == NULL) return;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    if (customisation == NULL) return;
    out_snapshot->layouts = customisation->layout_count; out_snapshot->available_windows = customisation->windows.count; out_snapshot->groups = customisation->groups.count; out_snapshot->presets = customisation->library.count; (void)snprintf(out_snapshot->active_layout_id,sizeof(out_snapshot->active_layout_id),"%s",customisation->active_layout_id); (void)snprintf(out_snapshot->theme_id,sizeof(out_snapshot->theme_id),"%s",customisation->theme.theme_id); out_snapshot->revision = customisation->revision;
    for (index = 0U; index < customisation->layout_count; ++index) if (strcmp(customisation->layouts[index].layout_id,customisation->active_layout_id) == 0) out_snapshot->active_layout_locked = customisation->layouts[index].locked;
}
