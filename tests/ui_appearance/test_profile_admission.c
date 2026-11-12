/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_profile_admission.c
 *
 * PURPOSE:
 *   Reject malformed appearance fields without changing an accepted catalogue profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/appearance.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); return 1; } } while (0)

static UmiStatus MakeProfile(UmiUiAppearanceProfile *profile)
{
    return umi_ui_appearance_profile_init(profile, "appearance-test", "Appearance test",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT);
}

/* Offsets let each fixed field be damaged independently. The surrounding
 * profile remains valid, exposing accidental scans into an adjacent field. */
#define FIELD(name) {#name, offsetof(UmiUiAppearanceProfile, name), sizeof(((UmiUiAppearanceProfile *)0)->name)}
static const struct { const char *name; size_t offset, size; } text_fields[] = {
    FIELD(profile_id), FIELD(label), FIELD(description), FIELD(brand_name),
    FIELD(logo_resource), FIELD(icon_resource), FIELD(interface_font), FIELD(editor_font),
    FIELD(background), FIELD(surface), FIELD(raised_surface), FIELD(hover_surface),
    FIELD(editor_background), FIELD(foreground), FIELD(muted_foreground), FIELD(border),
    FIELD(strong_border), FIELD(accent), FIELD(accent_surface), FIELD(success),
    FIELD(warning), FIELD(danger)
};
#undef FIELD
static int TextFields(void)
{
    for (size_t i = 0U; i < sizeof(text_fields) / sizeof(text_fields[0]); ++i) {
        UmiUiAppearanceProfile profile;
        char reason[192];
        CHECK(MakeProfile(&profile) == UMI_STATUS_OK);
        memset((unsigned char *)&profile + text_fields[i].offset, 'x', text_fields[i].size);
        fprintf(stderr, "Unterminated field: %s\n", text_fields[i].name);
        CHECK(umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
        CHECK(strstr(reason, "within each field") != NULL);
    }
    return 0;
}
static int Numbers(void)
{
    const double invalid[] = {NAN, INFINITY, -INFINITY};
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        for (unsigned field = 0U; field < 3U; ++field) {
            UmiUiAppearanceProfile profile;
            char reason[192];
            CHECK(MakeProfile(&profile) == UMI_STATUS_OK);
            if (field == 0U) profile.interface_font_size = invalid[i];
            else if (field == 1U) profile.editor_font_size = invalid[i];
            else profile.font_scale = invalid[i];
            CHECK(umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
        }
    }
    return 0;
}
static int Families(void)
{
    const char *invalid[] = {"", " ", ",Sans", "Sans,", "Sans, ,Mono", "...", "Sans; color:red", "Sans\"", "Sans\nMono"};
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        for (unsigned field = 0U; field < 2U; ++field) {
            UmiUiAppearanceProfile profile;
            char reason[192];
            CHECK(MakeProfile(&profile) == UMI_STATUS_OK);
            snprintf(field == 0U ? profile.interface_font : profile.editor_font,
                UMI_UI_APPEARANCE_FONT_CAPACITY, "%s", invalid[i]);
            CHECK(umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
        }
    }
    return 0;
}
static int Boundaries(void)
{
    UmiUiAppearanceProfile profile;
    char reason[192];
    CHECK(MakeProfile(&profile) == UMI_STATUS_OK);
    profile.interface_font_size = 8.0;
    profile.editor_font_size = 40.0;
    profile.font_scale = 0.75;
    snprintf(profile.interface_font, sizeof(profile.interface_font), " 3D.Font, My_Font-2, Sans ");
    CHECK(umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) == UMI_STATUS_OK);
    profile.interface_font_size = 32.0;
    profile.editor_font_size = 8.0;
    profile.font_scale = 2.0;
    CHECK(umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) == UMI_STATUS_OK);
    profile.font_scale = 2.0001;
    CHECK(umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    return 0;
}
static int PreserveModel(void)
{
    UmiUiAppearanceModel *model = NULL;
    UmiUiAppearanceProfile original, candidate, after;
    CHECK(MakeProfile(&original) == UMI_STATUS_OK);
    original.active = 1;
    CHECK(umi_ui_appearance_model_create(&model) == UMI_STATUS_OK);
    CHECK(umi_ui_appearance_model_upsert(model, &original) == UMI_STATUS_OK);
    CHECK(umi_ui_appearance_model_active(model, &original) == UMI_STATUS_OK);
    uint64_t revision = umi_ui_appearance_model_revision(model);
    candidate = original;
    candidate.editor_font_size = NAN;
    UmiStatus status = umi_ui_appearance_model_upsert(model, &candidate);
    UmiStatus read = umi_ui_appearance_model_active(model, &after);
    uint64_t after_revision = umi_ui_appearance_model_revision(model);
    umi_ui_appearance_model_destroy(model);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT && read == UMI_STATUS_OK);
    CHECK(revision == after_revision && memcmp(&original, &after, sizeof(original)) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "text") == 0) return TextFields();
    if (strcmp(argv[1], "numbers") == 0) return Numbers();
    if (strcmp(argv[1], "families") == 0) return Families();
    if (strcmp(argv[1], "boundaries") == 0) return Boundaries();
    if (strcmp(argv[1], "model") == 0) return PreserveModel();
    return 2;
}
