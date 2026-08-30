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

typedef enum UmiSettingType {
    UMI_SETTING_STRING = 0,
    UMI_SETTING_BOOLEAN = 1,
    UMI_SETTING_INTEGER = 2,
    UMI_SETTING_REAL = 3
} UmiSettingType;

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

typedef struct UmiSettings UmiSettings;

UmiStatus umi_settings_create(
    const UmiSettingDefinition *definitions,
    size_t definition_count,
    UmiSettings **out_settings
);

void umi_settings_destroy(UmiSettings *settings);

size_t umi_settings_count(const UmiSettings *settings);
int umi_settings_has(const UmiSettings *settings, const char *key);

UmiStatus umi_settings_definition_at(
    const UmiSettings *settings,
    size_t index,
    UmiSettingDefinitionView *out_definition
);

UmiStatus umi_settings_set_text(
    UmiSettings *settings,
    const char *key,
    const char *value
);

UmiStatus umi_settings_get_text(
    const UmiSettings *settings,
    const char *key,
    char *out_value,
    size_t out_value_capacity
);

UmiStatus umi_settings_set_boolean(
    UmiSettings *settings,
    const char *key,
    int value
);

UmiStatus umi_settings_get_boolean(
    const UmiSettings *settings,
    const char *key,
    int *out_value
);

UmiStatus umi_settings_set_integer(
    UmiSettings *settings,
    const char *key,
    int64_t value
);

UmiStatus umi_settings_get_integer(
    const UmiSettings *settings,
    const char *key,
    int64_t *out_value
);

UmiStatus umi_settings_set_real(
    UmiSettings *settings,
    const char *key,
    double value
);

UmiStatus umi_settings_get_real(
    const UmiSettings *settings,
    const char *key,
    double *out_value
);

UmiStatus umi_settings_reset(UmiSettings *settings, const char *key);
void umi_settings_reset_all(UmiSettings *settings);

UmiStatus umi_settings_load_file(UmiSettings *settings, const char *path);
UmiStatus umi_settings_save_file(const UmiSettings *settings, const char *path);

const char *umi_setting_type_text(UmiSettingType type);

#ifdef __cplusplus
}
#endif

#endif
