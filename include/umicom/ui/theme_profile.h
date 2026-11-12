/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/theme_profile.h
 *
 * PURPOSE:
 *   Publish the public theme profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_THEME_PROFILE_H
#define UMICOM_UI_THEME_PROFILE_H
#include "umicom/ui/workspace_layout.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named ui theme mode values accepted by this public contract.
 */
typedef enum UmiUiThemeMode {
  UMI_UI_THEME_MODE_SYSTEM = 1,
  UMI_UI_THEME_MODE_LIGHT,
  UMI_UI_THEME_MODE_DARK,
  UMI_UI_THEME_MODE_HIGH_CONTRAST
} UmiUiThemeMode;
/**
 * List the named ui density values accepted by this public contract.
 */
typedef enum UmiUiDensity {
  UMI_UI_DENSITY_COMPACT = 1,
  UMI_UI_DENSITY_COMFORTABLE,
  UMI_UI_DENSITY_SPACIOUS
} UmiUiDensity;
/**
 * Represent the ui theme profile data shared with callers of this public contract.
 */
typedef struct UmiUiThemeProfile {
  char theme_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
  char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
  UmiUiThemeMode mode;
  UmiUiDensity density;
  char accent[32U];
  char background[32U];
  char surface[32U];
  char foreground[32U];
  char success[32U];
  char warning[32U];
  char danger[32U];
  double font_scale;
  uint64_t revision;
} UmiUiThemeProfile;
/**
 * Initialise ui theme profile from caller-provided values so later operations receive a
 * known state.
 */
/** Initialize a complete canonical theme or leave the destination unchanged.
 * ID and name may borrow destination fields; each must be readable through its
 * terminator or field capacity. No text is truncated. Successful initialization
 * retains the established font scale of 1.0 and revision of one. */
UmiStatus umi_ui_theme_profile_init(UmiUiThemeProfile *theme, const char *theme_id,
                                    const char *name, UmiUiThemeMode mode, UmiUiDensity density);
/**
 * Check that ui theme profile satisfies its contract before another service relies on it.
 */
/* Validate bounded nonempty labels, declared mode/density, finite scale in
 * [0.75, 2.0] and all seven #RRGGBB fields. RGB letter case is permitted.
 * Rejection never edits the profile. Diagnostic output must be separate from
 * the profile; overlapping storage is INVALID_ARGUMENT without a write.
 * A short separate reason buffer remains terminated but returns
 * CAPACITY_EXCEEDED, preserving the established explanation-buffer contract.
 * Validation does not establish font installation, contrast or native rendering. */
UmiStatus umi_ui_theme_profile_validate(const UmiUiThemeProfile *theme, char *out_reason,
                                        size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
