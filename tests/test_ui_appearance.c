/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_appearance.c
 *
 * PURPOSE:
 *   Verify branded built-ins, activation, cloning and protection rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"
#include "value_archive/construction_cases.h"
#include "umicom/ui/brand_palette.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted appearance value. */
static int UmiUiAppearanceProfileConstructionCases(void)
{
    UmiUiAppearanceProfile *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ui_appearance_profile_init(value, "umicom-dark", "Umicom Dark", UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->profile_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ui_appearance_profile_init(value, oversized, "Umicom Dark", UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ui_appearance_profile_init(value, value->profile_id, "Umicom Dark", UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->profile_id, "umicom-dark") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->label) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ui_appearance_profile_init(value, "umicom-dark", oversized, UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ui_appearance_profile_init(value, "umicom-dark", value->label, UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->label, "Umicom Dark") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiUiAppearanceProfileConstructionCases() != 0) return 1;

    UmiUiAppearanceModel *model = NULL;
    UmiUiAppearanceProfile dark;
    UmiUiAppearanceProfile light;
    UmiUiAppearanceProfile result;
    const UmiUiBrandPalette *dark_palette;
    char reason[192U];

    assert(umi_ui_appearance_model_create(&model) == UMI_STATUS_OK);
    dark_palette = umi_ui_brand_palette_get(UMI_UI_BRAND_SURFACE_DARK);
    assert(dark_palette != NULL);
    assert(umi_ui_brand_palette_validate(dark_palette) == UMI_STATUS_OK);
    assert(umi_ui_appearance_profile_init(
               &dark, "umicom-dark", "Umicom Dark",
               UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) ==
           UMI_STATUS_OK);
    dark.built_in = 1;
    dark.active = 1;
    /* Appearance profiles project the canonical shared interaction accent. */
    assert(strcmp(dark.accent, dark_palette->accent) == 0);
    assert(strcmp(dark.surface, dark_palette->surface) == 0);
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
