/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/theme_profile.c
 *
 * PURPOSE:
 *   Initialise and validate named UI themes while sourcing all standard
 *   colours from the canonical Umicom brand palette.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/theme_profile.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ui/brand_palette.h"

/* Copy one colour into a fixed theme field and report truncation instead of
 * leaving a partly written token that an adapter could misinterpret. */
static UmiStatus copy_colour(char *destination, size_t capacity, const char *source) {
  int written;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (destination == NULL || capacity == 0U || source == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  written = snprintf(destination, capacity, "%s", source);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (written < 0 || (size_t)written >= capacity) {
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }
  return UMI_STATUS_OK;
}

/* System mode starts with the light palette until a frontend reports the
 * operating-system preference. This preserves the established Framework
 * behaviour while still letting the frontend switch to the dark palette. */
static UmiUiBrandSurface surface_for_mode(UmiUiThemeMode mode) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (mode) {
  case UMI_UI_THEME_MODE_LIGHT:
  case UMI_UI_THEME_MODE_SYSTEM:
    return UMI_UI_BRAND_SURFACE_LIGHT;
  case UMI_UI_THEME_MODE_HIGH_CONTRAST:
    return UMI_UI_BRAND_SURFACE_HIGH_CONTRAST;
  case UMI_UI_THEME_MODE_DARK:
  default:
    return UMI_UI_BRAND_SURFACE_DARK;
  }
}

/* Fill every colour field from one palette so related semantic values cannot
 * accidentally come from different light and dark variants. */
static UmiStatus apply_palette(UmiUiThemeProfile *theme) {
  const UmiUiBrandPalette *palette;
  UmiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (theme == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  palette = umi_ui_brand_palette_get(surface_for_mode(theme->mode));
  status = umi_ui_brand_palette_validate(palette);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;

#define COPY_THEME_COLOUR(field, value)                                                            \
  do {                                                                                             \
    status = copy_colour(theme->field, sizeof(theme->field), palette->value);                      \
    if (status != UMI_STATUS_OK)                                                                   \
      return status;                                                                               \
  } while (0)

  COPY_THEME_COLOUR(accent, accent);
  COPY_THEME_COLOUR(background, background);
  COPY_THEME_COLOUR(surface, surface);
  COPY_THEME_COLOUR(foreground, foreground);
  COPY_THEME_COLOUR(success, success);
  COPY_THEME_COLOUR(warning, warning);
  COPY_THEME_COLOUR(danger, danger);

#undef COPY_THEME_COLOUR

  return UMI_STATUS_OK;
}

/* Initialise a named profile and fill it with one internally consistent palette. */
UmiStatus umi_ui_theme_profile_init(UmiUiThemeProfile *theme, const char *theme_id,
                                    const char *name, UmiUiThemeMode mode, UmiUiDensity density) {
  int id_length;
  int name_length;

  /* Reject invalid identifiers and enum values before writing to the object,
   * so a failed call never leaves a partially initialised profile. */
  if (theme == NULL || theme_id == NULL || name == NULL || theme_id[0] == '\0' || name[0] == '\0' ||
      mode < UMI_UI_THEME_MODE_SYSTEM || mode > UMI_UI_THEME_MODE_HIGH_CONTRAST ||
      density < UMI_UI_DENSITY_COMPACT || density > UMI_UI_DENSITY_SPACIOUS) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }

  (void)memset(theme, 0, sizeof(*theme));
  id_length = snprintf(theme->theme_id, sizeof(theme->theme_id), "%s", theme_id);
  name_length = snprintf(theme->name, sizeof(theme->name), "%s", name);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (id_length < 0 || name_length < 0 || (size_t)id_length >= sizeof(theme->theme_id) ||
      (size_t)name_length >= sizeof(theme->name)) {
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }

  theme->mode = mode;
  theme->density = density;
  theme->font_scale = 1.0;
  theme->revision = 1U;
  return apply_palette(theme);
}

/* Write a short validation explanation without overflowing the caller's
 * buffer. This helper keeps every return path consistent. */
static UmiStatus write_reason(char *out_reason, size_t capacity, const char *reason,
                              UmiStatus result) {
  int written = snprintf(out_reason, capacity, "%s", reason);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (written < 0 || (size_t)written >= capacity) {
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }
  return result;
}

/* Validate user-adjustable theme values and return a readable explanation. */
UmiStatus umi_ui_theme_profile_validate(const UmiUiThemeProfile *theme, char *out_reason,
                                        size_t capacity) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (theme == NULL || out_reason == NULL || capacity == 0U) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }

  /* Very small or very large text scales can make controls unusable, so the
   * Framework keeps the profile within an accessible supported range. */
  if (theme->font_scale < 0.75 || theme->font_scale > 2.0) {
    return write_reason(out_reason, capacity, "Font scale must be between 0.75 and 2.0",
                        UMI_STATUS_INVALID_STATE);
  }

  /* Framework colour tokens use hexadecimal notation. Checking the leading
   * marker catches uninitialised fields and unsupported token formats. */
  if (theme->accent[0] != '#' || theme->background[0] != '#' || theme->surface[0] != '#' ||
      theme->foreground[0] != '#') {
    return write_reason(out_reason, capacity, "Theme colour tokens must use hexadecimal notation",
                        UMI_STATUS_INVALID_STATE);
  }

  return write_reason(out_reason, capacity, "Theme is valid", UMI_STATUS_OK);
}
