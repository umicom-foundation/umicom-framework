/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance.h
 *
 * PURPOSE:
 *   Define the reusable, toolkit-neutral appearance catalogue used by Umicom
 *   applications.  A profile owns semantic colour tokens, typography,
 *   density and accessibility preferences without exposing GTK, Qt or Wt.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Applications register built-in branded profiles and may clone one into a
 * user-owned profile.  Frontends only render the active snapshot, which keeps
 * the same theme choices available to desktop, web and future frontends.
 */

#ifndef UMICOM_UI_APPEARANCE_H
#define UMICOM_UI_APPEARANCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/theme_profile.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_APPEARANCE_MAX_PROFILES 48U
#define UMI_UI_APPEARANCE_FONT_CAPACITY 96U
#define UMI_UI_APPEARANCE_RESOURCE_CAPACITY 256U
#define UMI_UI_APPEARANCE_COLOUR_CAPACITY 16U

typedef struct UmiUiAppearanceProfile {
    char profile_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char label[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    UmiUiThemeMode mode;
    UmiUiDensity density;

    /* Brand metadata is deliberately a resource name/path rather than image
     * bytes.  Packaging remains an application responsibility. */
    char brand_name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    char logo_resource[UMI_UI_APPEARANCE_RESOURCE_CAPACITY];
    char icon_resource[UMI_UI_APPEARANCE_RESOURCE_CAPACITY];

    char interface_font[UMI_UI_APPEARANCE_FONT_CAPACITY];
    char editor_font[UMI_UI_APPEARANCE_FONT_CAPACITY];
    double interface_font_size;
    double editor_font_size;
    double font_scale;

    /* Semantic tokens prevent widget names from leaking into the model. */
    char background[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char surface[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char raised_surface[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char hover_surface[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char editor_background[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char foreground[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char muted_foreground[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char border[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char strong_border[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char accent[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char accent_surface[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char success[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char warning[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char danger[UMI_UI_APPEARANCE_COLOUR_CAPACITY];

    int active;
    int built_in;
    int locked;
    int reduce_motion;
    uint64_t revision;
} UmiUiAppearanceProfile;

typedef struct UmiUiAppearanceModel UmiUiAppearanceModel;

UmiStatus umi_ui_appearance_profile_init(
    UmiUiAppearanceProfile *profile,
    const char *profile_id,
    const char *label,
    UmiUiThemeMode mode,
    UmiUiDensity density);
UmiStatus umi_ui_appearance_profile_validate(
    const UmiUiAppearanceProfile *profile,
    char *out_reason,
    size_t capacity);

UmiStatus umi_ui_appearance_model_create(UmiUiAppearanceModel **out_model);
void umi_ui_appearance_model_destroy(UmiUiAppearanceModel *model);
UmiStatus umi_ui_appearance_model_upsert(
    UmiUiAppearanceModel *model,
    const UmiUiAppearanceProfile *profile);
UmiStatus umi_ui_appearance_model_remove(
    UmiUiAppearanceModel *model,
    const char *profile_id);
UmiStatus umi_ui_appearance_model_find(
    const UmiUiAppearanceModel *model,
    const char *profile_id,
    UmiUiAppearanceProfile *out_profile);
UmiStatus umi_ui_appearance_model_at(
    const UmiUiAppearanceModel *model,
    size_t index,
    UmiUiAppearanceProfile *out_profile);
UmiStatus umi_ui_appearance_model_active(
    const UmiUiAppearanceModel *model,
    UmiUiAppearanceProfile *out_profile);
UmiStatus umi_ui_appearance_model_set_active(
    UmiUiAppearanceModel *model,
    const char *profile_id);
UmiStatus umi_ui_appearance_model_clone(
    UmiUiAppearanceModel *model,
    const char *source_profile_id,
    const char *new_profile_id,
    const char *new_label);
UmiStatus umi_ui_appearance_model_set_locked(
    UmiUiAppearanceModel *model,
    const char *profile_id,
    int locked);
size_t umi_ui_appearance_model_count(const UmiUiAppearanceModel *model);
uint64_t umi_ui_appearance_model_revision(const UmiUiAppearanceModel *model);

#ifdef __cplusplus
}
#endif

#endif
