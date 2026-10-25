/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/settings_changes.inc
 *
 * PURPOSE:
 *   Keep multi-setting validation and comparison inside the existing settings lock.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/settings_changes.h"

UmiStatus UmiSettingsReadValues(const UmiSettings *settings, const char *const *keys, size_t count,
                                UmiSettingValue *out_values) {
  if (settings == NULL || keys == NULL || out_values == NULL || count == 0U ||
      count > UMI_SETTINGS_MAX_DEFINITIONS)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t indices[UMI_SETTINGS_MAX_DEFINITIONS];
  for (size_t i = 0U; i < count; ++i) {
    if (keys[i] == NULL)
      return UMI_STATUS_INVALID_ARGUMENT;
    if (!find_index(settings, keys[i], &indices[i]))
      return UMI_STATUS_NOT_FOUND;
    for (size_t j = 0U; j < i; ++j)
      if (indices[j] == indices[i])
        return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Copy after complete admission. There are no fallible operations once
   * the lock is held, so readers cannot observe half of a related pair. */
  settings_lock(settings);
  for (size_t i = 0U; i < count; ++i)
    memcpy(out_values[i].text, settings->entries[indices[i]].value, sizeof out_values[i].text);
  settings_unlock(settings);
  return UMI_STATUS_OK;
}

typedef struct PreparedSettingChange {
  size_t index;
  char expected[UMI_SETTING_VALUE_CAPACITY];
  char candidate[UMI_SETTING_VALUE_CAPACITY];
} PreparedSettingChange;
UmiStatus UmiSettingsApplyChanges(UmiSettings *settings, const UmiSettingChange *changes,
                                  size_t count) {
  if (settings == NULL || changes == NULL || count == 0U || count > UMI_SETTINGS_MAX_DEFINITIONS)
    return UMI_STATUS_INVALID_ARGUMENT;
  PreparedSettingChange *prepared = calloc(count, sizeof *prepared);
  if (prepared == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = UMI_STATUS_OK;
  /* Schema metadata is immutable. Allocate and normalise outside the short
   * lock; only comparison and non-failing publication occur inside it. */
  for (size_t i = 0U; i < count && status == UMI_STATUS_OK; ++i) {
    if (changes[i].key == NULL || changes[i].expected_value == NULL || changes[i].value == NULL) {
      status = UMI_STATUS_INVALID_ARGUMENT;
      break;
    }
    if (!find_index(settings, changes[i].key, &prepared[i].index)) {
      status = UMI_STATUS_NOT_FOUND;
      break;
    }
    for (size_t j = 0U; j < i; ++j)
      if (prepared[j].index == prepared[i].index)
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK)
      break;
    const UmiSettingEntry *entry = &settings->entries[prepared[i].index];
    status = normalise_value(entry, changes[i].expected_value, prepared[i].expected,
                             sizeof prepared[i].expected);
    if (status == UMI_STATUS_OK)
      status = normalise_value(entry, changes[i].value, prepared[i].candidate,
                               sizeof prepared[i].candidate);
  }
  if (status == UMI_STATUS_OK) {
    settings_lock(settings);
    for (size_t i = 0U; i < count; ++i)
      if (strcmp(settings->entries[prepared[i].index].value, prepared[i].expected) != 0) {
        status = UMI_STATUS_INVALID_STATE;
        break;
      }
    if (status == UMI_STATUS_OK)
      for (size_t i = 0U; i < count; ++i)
        memcpy(settings->entries[prepared[i].index].value, prepared[i].candidate,
               sizeof prepared[i].candidate);
    settings_unlock(settings);
  }
  free(prepared);
  return status;
}
