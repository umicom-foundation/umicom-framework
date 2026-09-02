/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/settings.h
 *
 * PURPOSE:
 *   Provide a reusable typed settings repository for every Umicom application.
 *   A schema defines recognised keys, value types, defaults, descriptions and
 *   optional numeric ranges.  The repository validates values before they are
 *   stored and persists the current values in a small human-readable file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_SETTINGS_H
#define UMICOM_PLATFORM_SETTINGS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SETTING_KEY_CAPACITY 128U
#define UMI_SETTING_VALUE_CAPACITY 512U
#define UMI_SETTING_DESCRIPTION_CAPACITY 256U
#define UMI_SETTINGS_MAX_DEFINITIONS 256U

/**
 * List the named setting type values accepted by this public contract.
 */
typedef enum UmiSettingType {
    UMI_SETTING_STRING = 0,
    UMI_SETTING_BOOLEAN = 1,
    UMI_SETTING_INTEGER = 2,
    UMI_SETTING_REAL = 3
} UmiSettingType;

/**
 * Represent the setting definition data shared with callers of this public contract.
 */
typedef struct UmiSettingDefinition {
    const char *key;
    UmiSettingType type;
    const char *default_value;
    const char *description;
    int64_t minimum_integer;
    int64_t maximum_integer;
    double minimum_real;
    double maximum_real;
    int has_integer_range;
    int has_real_range;
} UmiSettingDefinition;

/**
 * Represent the setting definition view data shared with callers of this public contract.
 */
typedef struct UmiSettingDefinitionView {
    const char *key;
    UmiSettingType type;
    const char *default_value;
    const char *description;
    int64_t minimum_integer;
    int64_t maximum_integer;
    double minimum_real;
    double maximum_real;
    int has_integer_range;
    int has_real_range;
} UmiSettingDefinitionView;

/**
 * Represent the settings data shared with callers of this public contract.
 */
typedef struct UmiSettings UmiSettings;

/**
 * Initialise settings from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_settings_create(
    const UmiSettingDefinition *definitions,
    size_t definition_count,
    UmiSettings **out_settings
);

/**
 * Release or reset state held by settings so the same storage can be reused safely.
 */
void umi_settings_destroy(UmiSettings *settings);

/**
 * Return the number of records represented by settings without changing their state.
 */
size_t umi_settings_count(const UmiSettings *settings);
/**
 * Provide the settings has operation used by this module and its client applications.
 */
int umi_settings_has(const UmiSettings *settings, const char *key);

/**
 * Find settings definition while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_settings_definition_at(
    const UmiSettings *settings,
    size_t index,
    UmiSettingDefinitionView *out_definition
);

/**
 * Provide the settings set text operation used by this module and its client applications.
 */
UmiStatus umi_settings_set_text(
    UmiSettings *settings,
    const char *key,
    const char *value
);

/**
 * Provide the settings get text operation used by this module and its client applications.
 */
UmiStatus umi_settings_get_text(
    const UmiSettings *settings,
    const char *key,
    char *out_value,
    size_t out_value_capacity
);

/**
 * Provide the settings set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_set_boolean(
    UmiSettings *settings,
    const char *key,
    int value
);

/**
 * Provide the settings get boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_get_boolean(
    const UmiSettings *settings,
    const char *key,
    int *out_value
);

/**
 * Provide the settings set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_set_integer(
    UmiSettings *settings,
    const char *key,
    int64_t value
);

/**
 * Provide the settings get integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_get_integer(
    const UmiSettings *settings,
    const char *key,
    int64_t *out_value
);

/**
 * Provide the settings set real operation used by this module and its client applications.
 */
UmiStatus umi_settings_set_real(
    UmiSettings *settings,
    const char *key,
    double value
);

/**
 * Provide the settings get real operation used by this module and its client applications.
 */
UmiStatus umi_settings_get_real(
    const UmiSettings *settings,
    const char *key,
    double *out_value
);

/**
 * Release or reset state held by settings so the same storage can be reused safely.
 */
UmiStatus umi_settings_reset(UmiSettings *settings, const char *key);
/**
 * Provide the settings reset all operation used by this module and its client
 * applications.
 */
void umi_settings_reset_all(UmiSettings *settings);

/**
 * Provide the settings load file operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_load_file(UmiSettings *settings, const char *path);
/**
 * Provide the settings save file operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_save_file(const UmiSettings *settings, const char *path);

/**
 * Provide the setting type text operation used by this module and its client applications.
 */
const char *umi_setting_type_text(UmiSettingType type);

#ifdef __cplusplus
}
#endif

#endif
