/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_profile.h
 *
 * PURPOSE:
 *   Define reusable named workbench chrome profiles. A profile describes the
 *   visibility and preferred size of the primary, auxiliary and bottom tool
 *   regions without depending on GTK, Qt, Wt or any other frontend toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A workspace profile is similar to a saved IDE layout tab. Applications add
 * profiles such as Develop, Focus or Debug; frontend adapters render a picker
 * and the Framework remains the authoritative owner of the selected profile.
 */

#ifndef UMICOM_UI_WORKSPACE_PROFILE_H
#define UMICOM_UI_WORKSPACE_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WORKSPACE_PROFILE_MAX 64U

typedef struct UmiUiWorkspaceProfileSnapshot {
    char profile_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    int sidebar_visible;
    int auxiliary_sidebar_visible;
    int bottom_panel_visible;
    int32_t sidebar_size;
    int32_t auxiliary_sidebar_size;
    int32_t bottom_panel_size;
    int32_t order;
    int active;
    int built_in;
} UmiUiWorkspaceProfileSnapshot;

typedef struct UmiUiWorkspaceProfileModel UmiUiWorkspaceProfileModel;

UmiStatus umi_ui_workspace_profile_model_create(
    UmiUiWorkspaceProfileModel **out_model);
void umi_ui_workspace_profile_model_destroy(UmiUiWorkspaceProfileModel *model);
UmiStatus umi_ui_workspace_profile_model_upsert(
    UmiUiWorkspaceProfileModel *model,
    const UmiUiWorkspaceProfileSnapshot *profile);
UmiStatus umi_ui_workspace_profile_model_remove(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id);
UmiStatus umi_ui_workspace_profile_model_find(
    const UmiUiWorkspaceProfileModel *model,
    const char *profile_id,
    UmiUiWorkspaceProfileSnapshot *out_profile);
UmiStatus umi_ui_workspace_profile_model_at(
    const UmiUiWorkspaceProfileModel *model,
    size_t index,
    UmiUiWorkspaceProfileSnapshot *out_profile);
UmiStatus umi_ui_workspace_profile_model_set_active(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id);
size_t umi_ui_workspace_profile_model_count(
    const UmiUiWorkspaceProfileModel *model);
uint64_t umi_ui_workspace_profile_model_revision(
    const UmiUiWorkspaceProfileModel *model);

#ifdef __cplusplus
}
#endif

#endif
