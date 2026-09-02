/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance.h
 *
 * PURPOSE:
 *   Define the reusable, toolkit-neutral appearance catalogue used by Umicom
 *   applications.  A profile owns semantic colour tokens, typography,
 *   density and accessibility preferences without exposing GTK, Qt or Wt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Applications register built-in branded profiles and may clone one into a
 * user-owned profile.  Frontends only render the active snapshot, which keeps
 * the same theme choices available to desktop, web and future frontends.
 */

#ifndef UMICOM_UI_APPEARANCE_H
#define UMICOM_UI_APPEARANCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/brand_palette.h"
#include "umicom/ui/theme_profile.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_APPEARANCE_MAX_PROFILES 48U
#define UMI_UI_APPEARANCE_FONT_CAPACITY 96U
#define UMI_UI_APPEARANCE_RESOURCE_CAPACITY 256U
#define UMI_UI_APPEARANCE_COLOUR_CAPACITY 16U

/**
 * Represent the ui appearance profile data shared with callers of this public contract.
 */
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

/**
 * Represent the ui appearance model data shared with callers of this public contract.
 */
typedef struct UmiUiAppearanceModel UmiUiAppearanceModel;

/**
 * Initialise ui appearance profile from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_appearance_profile_init(
    UmiUiAppearanceProfile *profile,
    const char *profile_id,
    const char *label,
    UmiUiThemeMode mode,
    UmiUiDensity density);
/**
 * Check that ui appearance profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ui_appearance_profile_validate(
    const UmiUiAppearanceProfile *profile,
    char *out_reason,
    size_t capacity);

/**
 * Initialise ui appearance model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_appearance_model_create(UmiUiAppearanceModel **out_model);
/**
 * Release or reset state held by ui appearance model so the same storage can be reused
 * safely.
 */
void umi_ui_appearance_model_destroy(UmiUiAppearanceModel *model);
/**
 * Provide the ui appearance model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_appearance_model_upsert(
    UmiUiAppearanceModel *model,
    const UmiUiAppearanceProfile *profile);
/**
 * Remove ui appearance model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_appearance_model_remove(
    UmiUiAppearanceModel *model,
    const char *profile_id);
/**
 * Find ui appearance model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_appearance_model_find(
    const UmiUiAppearanceModel *model,
    const char *profile_id,
    UmiUiAppearanceProfile *out_profile);
/**
 * Find ui appearance model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_appearance_model_at(
    const UmiUiAppearanceModel *model,
    size_t index,
    UmiUiAppearanceProfile *out_profile);
/**
 * Provide the ui appearance model active operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_appearance_model_active(
    const UmiUiAppearanceModel *model,
    UmiUiAppearanceProfile *out_profile);
/**
 * Provide the ui appearance model set active operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_appearance_model_set_active(
    UmiUiAppearanceModel *model,
    const char *profile_id);
/**
 * Provide the ui appearance model clone operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_appearance_model_clone(
    UmiUiAppearanceModel *model,
    const char *source_profile_id,
    const char *new_profile_id,
    const char *new_label);
/**
 * Provide the ui appearance model set locked operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_appearance_model_set_locked(
    UmiUiAppearanceModel *model,
    const char *profile_id,
    int locked);
/**
 * Return the number of records represented by ui appearance model without changing their
 * state.
 */
size_t umi_ui_appearance_model_count(const UmiUiAppearanceModel *model);
/**
 * Provide the ui appearance model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_appearance_model_revision(const UmiUiAppearanceModel *model);

#ifdef __cplusplus
}
#endif

#endif
