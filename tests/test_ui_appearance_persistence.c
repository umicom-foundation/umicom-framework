/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_appearance_persistence.c
 *
 * PURPOSE:
 *   Verify lossless, validated appearance-profile persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiAppearanceProfile source;
    UmiUiAppearanceProfile restored;
    char encoded[UMI_UI_APPEARANCE_TEXT_CAPACITY];

    assert(umi_ui_appearance_profile_init(
               &source, "sammy-dark", "Sammy | Dark",
               UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) ==
           UMI_STATUS_OK);
    (void)snprintf(source.description, sizeof(source.description), "%s",
                   "A saved theme with punctuation: red | navy");
    (void)snprintf(source.logo_resource, sizeof(source.logo_resource), "%s",
                   "branding/umicom-logo.svg");
    (void)snprintf(source.icon_resource, sizeof(source.icon_resource), "%s",
                   "branding/umicom-icon.svg");
    source.editor_font_size = 12.5;
    source.font_scale = 1.125;
    source.active = 1;
    source.revision = 42U;

    assert(umi_ui_appearance_profile_encode(
               &source, encoded, sizeof(encoded)) == UMI_STATUS_OK);
    assert(strstr(encoded, "%7C") != NULL);
    assert(umi_ui_appearance_profile_decode(encoded, &restored) ==
           UMI_STATUS_OK);
    assert(strcmp(restored.profile_id, source.profile_id) == 0);
    assert(strcmp(restored.label, source.label) == 0);
    assert(strcmp(restored.description, source.description) == 0);
    assert(strcmp(restored.logo_resource, source.logo_resource) == 0);
    assert(restored.editor_font_size == 12.5);
    assert(restored.font_scale == 1.125);
    assert(restored.active && restored.revision == 42U);
    assert(umi_ui_appearance_profile_decode(
               "appearance-v1|broken", &restored) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_ui_appearance_profile_decode(
               "appearance-v1|broken%", &restored) ==
           UMI_STATUS_INVALID_STATE);
    return EXIT_SUCCESS;
}
