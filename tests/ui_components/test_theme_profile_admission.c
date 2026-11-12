/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_theme_profile_admission.c
 *
 * PURPOSE:
 *   Verify finite typography, complete RGB fields and non-mutating diagnostic ownership for theme profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

#include "umicom/ui/theme_profile.h"

int main(void)
{
    UmiUiThemeProfile valid = {0}, value, before;
    char reason[256];
    CHECK(umi_ui_theme_profile_init(&valid, "umicom.dark", "Umicom Dark",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_profile_validate(&valid, reason, sizeof(reason)) == UMI_STATUS_OK);
    const double invalid_scales[] = {NAN, INFINITY, -INFINITY, 0.74, 2.01};
    for (size_t i = 0U; i < sizeof(invalid_scales) / sizeof(invalid_scales[0]); ++i) {
        value = valid;
        value.font_scale = invalid_scales[i];
        memcpy(&before, &value, sizeof(before));
        CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&before, &value, sizeof(before)) == 0);
    }
    const double endpoints[] = {0.75, 2.0};
    for (size_t i = 0U; i < sizeof(endpoints) / sizeof(endpoints[0]); ++i) {
        value = valid;
        value.font_scale = endpoints[i];
        CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_OK);
    }
    const size_t colours[] = {
        offsetof(UmiUiThemeProfile, accent), offsetof(UmiUiThemeProfile, background),
        offsetof(UmiUiThemeProfile, surface), offsetof(UmiUiThemeProfile, foreground),
        offsetof(UmiUiThemeProfile, success), offsetof(UmiUiThemeProfile, warning),
        offsetof(UmiUiThemeProfile, danger)
    };
    for (size_t i = 0U; i < sizeof(colours) / sizeof(colours[0]); ++i) {
        value = valid;
        char *field = (char *)&value + colours[i];
        memset(field, '#', sizeof(value.accent));
        CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
        (void)snprintf(field, sizeof(value.accent), "%s", "#12x456");
        CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
        (void)snprintf(field, sizeof(value.accent), "%s", "#abc");
        CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
        (void)snprintf(field, sizeof(value.accent), "%s", "#12aBcF");
        CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_OK);
    }
    value = valid;
    memset(value.theme_id, 'i', sizeof(value.theme_id));
    CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    value = valid;
    memset(value.name, 'n', sizeof(value.name));
    CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    value = valid;
    value.theme_id[0] = '\0';
    CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    value = valid;
    value.mode = (UmiUiThemeMode)0;
    CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    value = valid;
    value.density = (UmiUiDensity)99;
    CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    value = valid;
    memcpy(&before, &value, sizeof(before));
    CHECK(umi_ui_theme_profile_validate(&value, value.name, sizeof(value.name)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &value, sizeof(before)) == 0);
    char tiny[1] = {'x'};
    CHECK(umi_ui_theme_profile_validate(&value, tiny, sizeof(tiny)) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(tiny[0] == '\0');
    CHECK(umi_ui_theme_profile_validate(NULL, reason, sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_profile_validate(&value, NULL, sizeof(reason)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_profile_validate(&value, reason, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
