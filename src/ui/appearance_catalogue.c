/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance_catalogue.c
 *
 * PURPOSE:
 *   Build the shared light, dark, high-contrast, retro, neo and editable
 *   appearance profiles used by Umicom application frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/appearance_catalogue.h"

#include <stdio.h>
#include <string.h>

#define UMI_UI_APPEARANCE_STANDARD_COUNT 6U

/* Copy text into a fixed profile field and reject truncation. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source) {
  int written;

  if (destination == NULL || capacity == 0U || source == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  written = snprintf(destination, capacity, "%s", source);
  return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/* Replace all semantic colours as one operation so a preset cannot mix two
 * visual families by accident. */
static UmiStatus set_colours(UmiUiAppearanceProfile *profile, const char *background,
                             const char *surface, const char *raised_surface,
                             const char *hover_surface, const char *editor_background,
                             const char *foreground, const char *muted_foreground,
                             const char *border, const char *strong_border, const char *accent,
                             const char *accent_surface, const char *success, const char *warning,
                             const char *danger) {
#define COPY_COLOUR(field, value)                                                                  \
  do {                                                                                             \
    UmiStatus field_status = copy_text(profile->field, sizeof(profile->field), value);             \
    if (field_status != UMI_STATUS_OK)                                                             \
      return field_status;                                                                         \
  } while (0)

  if (profile == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  COPY_COLOUR(background, background);
  COPY_COLOUR(surface, surface);
  COPY_COLOUR(raised_surface, raised_surface);
  COPY_COLOUR(hover_surface, hover_surface);
  COPY_COLOUR(editor_background, editor_background);
  COPY_COLOUR(foreground, foreground);
  COPY_COLOUR(muted_foreground, muted_foreground);
  COPY_COLOUR(border, border);
  COPY_COLOUR(strong_border, strong_border);
  COPY_COLOUR(accent, accent);
  COPY_COLOUR(accent_surface, accent_surface);
  COPY_COLOUR(success, success);
  COPY_COLOUR(warning, warning);
  COPY_COLOUR(danger, danger);
  return UMI_STATUS_OK;

#undef COPY_COLOUR
}

/* Create one catalogue row and mark immutable presets as built in. */
static UmiStatus make_profile(size_t index, UmiUiAppearanceProfile *out_profile) {
  UmiUiAppearanceProfile profile;
  UmiStatus status;

  if (out_profile == NULL || index >= UMI_UI_APPEARANCE_STANDARD_COUNT) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }

  switch (index) {
  case 0U:
    status = umi_ui_appearance_profile_init(&profile, "umicom-dark", "Dark", UMI_UI_THEME_MODE_DARK,
                                            UMI_UI_DENSITY_COMFORTABLE);
    break;
  case 1U:
    status = umi_ui_appearance_profile_init(&profile, "umicom-light", "Light",
                                            UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_COMFORTABLE);
    break;
  case 2U:
    status =
        umi_ui_appearance_profile_init(&profile, "umicom-high-contrast", "High Contrast",
                                       UMI_UI_THEME_MODE_HIGH_CONTRAST, UMI_UI_DENSITY_COMFORTABLE);
    break;
  case 3U:
    status = umi_ui_appearance_profile_init(&profile, "umicom-retro", "Retro",
                                            UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT);
    if (status == UMI_STATUS_OK) {
      status = set_colours(&profile, "#11150F", "#192016", "#242D20", "#303B2A", "#0D110C",
                           "#DDE8B8", "#9EAD7F", "#405036", "#60734D", "#D39B38", "#493514",
                           "#75B86B", "#E2B64F", "#D76C5D");
    }
    if (status == UMI_STATUS_OK) {
      status =
          copy_text(profile.interface_font, sizeof(profile.interface_font), "Consolas, Monospace");
    }
    break;
  case 4U:
    status = umi_ui_appearance_profile_init(&profile, "umicom-neo", "Neo", UMI_UI_THEME_MODE_DARK,
                                            UMI_UI_DENSITY_COMPACT);
    if (status == UMI_STATUS_OK) {
      status = set_colours(&profile, "#0E1020", "#171A2E", "#202642", "#2A3152", "#0B0D18",
                           "#EDF2FF", "#A4AEC8", "#303956", "#4B5A82", "#6FD3FF", "#153C55",
                           "#62D5A3", "#F2C46D", "#FF758F");
    }
    break;
  case 5U:
    status = umi_ui_appearance_profile_init(&profile, "umicom-custom", "Custom",
                                            UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMFORTABLE);
    break;
  default:
    return UMI_STATUS_NOT_FOUND;
  }
  if (status != UMI_STATUS_OK)
    return status;

  /* Each appearance points to a contrast-aware Framework asset. Applications
   * can therefore render the same mark without selecting their own file. */
  status = copy_text(profile.logo_resource, sizeof(profile.logo_resource),
                     profile.mode == UMI_UI_THEME_MODE_LIGHT ? "branding/umicom-logo.svg"
                                                             : "branding/umicom-logo-on-dark.svg");
  if (status == UMI_STATUS_OK) {
    status =
        copy_text(profile.icon_resource, sizeof(profile.icon_resource),
                  profile.mode == UMI_UI_THEME_MODE_LIGHT ? "branding/umicom-icon.svg"
                                                          : "branding/umicom-icon-on-dark.svg");
  }
  if (status != UMI_STATUS_OK)
    return status;

  /* The first profile is the predictable default. Custom remains editable
   * while catalogue presets are protected from accidental replacement. */
  profile.active = index == 0U;
  profile.built_in = index + 1U < UMI_UI_APPEARANCE_STANDARD_COUNT;
  profile.locked = profile.built_in;
  status = copy_text(
      profile.description, sizeof(profile.description),
      index == 3U
          ? "Warm compact controls inspired by classic terminal workspaces."
          : (index == 4U ? "Cool high-clarity accents for dense analytical workspaces."
                         : (index == 5U ? "A user-owned profile for colours, fonts and sizing."
                                        : "A balanced Umicom application appearance.")));
  if (status != UMI_STATUS_OK)
    return status;
  *out_profile = profile;
  return UMI_STATUS_OK;
}

/* Return the stable catalogue size used by selectors and tests. */
size_t umi_ui_appearance_catalogue_count(void) { return UMI_UI_APPEARANCE_STANDARD_COUNT; }

/* Materialise one profile on demand so callers receive independent values. */
UmiStatus umi_ui_appearance_catalogue_at(size_t index, UmiUiAppearanceProfile *out_profile) {
  if (index >= UMI_UI_APPEARANCE_STANDARD_COUNT) {
    return UMI_STATUS_NOT_FOUND;
  }
  return make_profile(index, out_profile);
}

/* Search catalogue identifiers without returning internal mutable storage. */
UmiStatus umi_ui_appearance_catalogue_find(const char *profile_id,
                                           UmiUiAppearanceProfile *out_profile) {
  size_t index;
  UmiUiAppearanceProfile profile;
  UmiStatus status;

  if (profile_id == NULL || out_profile == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  for (index = 0U; index < UMI_UI_APPEARANCE_STANDARD_COUNT; ++index) {
    status = make_profile(index, &profile);
    if (status != UMI_STATUS_OK)
      return status;
    if (strcmp(profile.profile_id, profile_id) == 0) {
      *out_profile = profile;
      return UMI_STATUS_OK;
    }
  }
  return UMI_STATUS_NOT_FOUND;
}

/* Populate a model in display order and preserve the dark default. */
UmiStatus umi_ui_appearance_catalogue_populate(UmiUiAppearanceModel *model) {
  size_t index;
  UmiUiAppearanceProfile profile;
  UmiStatus status;

  if (model == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (index = 0U; index < UMI_UI_APPEARANCE_STANDARD_COUNT; ++index) {
    status = make_profile(index, &profile);
    if (status != UMI_STATUS_OK)
      return status;
    status = umi_ui_appearance_model_upsert(model, &profile);
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}
