/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/settings_changes.h
 *
 * PURPOSE:
 *   Read related settings consistently and publish reviewed changes as one checked operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_SETTINGS_CHANGES_H
#define UMICOM_PLATFORM_SETTINGS_CHANGES_H
#include "umicom/platform/settings.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSettingValue {
  char text[UMI_SETTING_VALUE_CAPACITY];
} UmiSettingValue;
typedef struct UmiSettingChange {
  const char *key;
  const char *expected_value;
  const char *value;
} UmiSettingChange;
/** Copy values under one repository lock. On failure out_values is untouched.
 * Keys must be distinct. count is 1..UMI_SETTINGS_MAX_DEFINITIONS. All input
 * strings and output storage belong to the caller and must not overlap. */
UmiStatus UmiSettingsReadValues(const UmiSettings *settings, const char *const *keys, size_t count,
                                UmiSettingValue *out_values);
/** Validate every candidate and expected value against the existing schema,
 * then compare and publish all values under one lock. A mismatch returns
 * INVALID_STATE without changing any setting. No disk writes or callbacks.
 * Expected values compare after schema normalisation. This compares values,
 * not history: changing a value away and back does not invalidate a review.
 * Caller owns stable inputs for the duration of this synchronous operation. */
UmiStatus UmiSettingsApplyChanges(UmiSettings *settings, const UmiSettingChange *changes,
                                  size_t count);
#ifdef __cplusplus
}
#endif
#endif
