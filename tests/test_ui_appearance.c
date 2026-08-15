/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_appearance.c
 *
 * PURPOSE:
 *   Verify branded built-ins, activation, cloning and protection rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiUiAppearanceModel *model = NULL;
    UmiUiAppearanceProfile dark;
    UmiUiAppearanceProfile light;
    UmiUiAppearanceProfile result;
    char reason[192U];

    assert(umi_ui_appearance_model_create(&model) == UMI_STATUS_OK);
    assert(umi_ui_appearance_profile_init(
               &dark, "umicom-dark", "Umicom Dark",
               UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) ==
           UMI_STATUS_OK);
    dark.built_in = 1;
    dark.active = 1;
    assert(strcmp(dark.accent, "#C84C55") == 0);
    assert(strcmp(dark.surface, "#1D2731") == 0);
    assert(umi_ui_appearance_profile_validate(
               &dark, reason, sizeof(reason)) == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_upsert(model, &dark) == UMI_STATUS_OK);

    assert(umi_ui_appearance_profile_init(
               &light, "umicom-light", "Umicom Light",
               UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_COMFORTABLE) ==
           UMI_STATUS_OK);
    light.built_in = 1;
    assert(umi_ui_appearance_model_upsert(model, &light) == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_count(model) == 2U);
    assert(umi_ui_appearance_model_active(model, &result) == UMI_STATUS_OK);
    assert(strcmp(result.profile_id, "umicom-dark") == 0);

    /* Upserting an active user profile keeps the catalogue's single-active
     * invariant without making the caller deactivate every other profile. */
    light.built_in = 0;
    light.active = 1;
    (void)snprintf(light.profile_id, sizeof(light.profile_id), "%s",
                   "preview-light");
    assert(umi_ui_appearance_model_upsert(model, &light) == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_active(model, &result) == UMI_STATUS_OK);
    assert(strcmp(result.profile_id, "preview-light") == 0);
    light.active = 0;
    assert(umi_ui_appearance_model_upsert(model, &light) == UMI_STATUS_OK);

    assert(umi_ui_appearance_model_set_active(model, "umicom-light") ==
           UMI_STATUS_OK);
    assert(umi_ui_appearance_model_active(model, &result) == UMI_STATUS_OK);
    assert(strcmp(result.profile_id, "umicom-light") == 0);
    assert(umi_ui_appearance_model_remove(model, "umicom-light") ==
           UMI_STATUS_PERMISSION_DENIED);

    assert(umi_ui_appearance_model_clone(
               model, "umicom-dark", "my-dark", "My Dark") ==
           UMI_STATUS_OK);
    assert(umi_ui_appearance_model_find(model, "my-dark", &result) ==
           UMI_STATUS_OK);
    assert(!result.built_in && !result.locked && !result.active);
    assert(umi_ui_appearance_model_set_locked(model, "my-dark", 1) ==
           UMI_STATUS_OK);
    assert(umi_ui_appearance_model_remove(model, "my-dark") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ui_appearance_model_set_locked(model, "my-dark", 0) ==
           UMI_STATUS_OK);
    assert(umi_ui_appearance_model_remove(model, "my-dark") == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_remove(model, "preview-light") ==
           UMI_STATUS_OK);

    umi_ui_appearance_model_destroy(model);
    return EXIT_SUCCESS;
}
