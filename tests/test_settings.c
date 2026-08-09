/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_settings.c
 *
 * PURPOSE:
 *   Verify schema validation, typed access, range enforcement, reset behaviour,
 *   forward-compatible file loading and persistence for UmiSettings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/settings.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static const UmiSettingDefinition TEST_SCHEMA[] = {
    {
        "test.name",
        UMI_SETTING_STRING,
        "Umicom",
        "A string value.",
        0,
        0,
        0.0,
        0.0,
        0,
        0
    },
    {
        "test.enabled",
        UMI_SETTING_BOOLEAN,
        "true",
        "A boolean value.",
        0,
        0,
        0.0,
        0.0,
        0,
        0
    },
    {
        "test.count",
        UMI_SETTING_INTEGER,
        "4",
        "A ranged integer.",
        1,
        16,
        0.0,
        0.0,
        1,
        0
    },
    {
        "test.ratio",
        UMI_SETTING_REAL,
        "0.5",
        "A ranged real value.",
        0,
        0,
        0.0,
        1.0,
        0,
        1
    }
};

int main(void)
{
    UmiSettings *settings = NULL;
    UmiSettings *loaded = NULL;
    UmiSettingDefinitionView definition;
    char text[UMI_SETTING_VALUE_CAPACITY];
    int boolean_value = 0;
    int64_t integer_value = 0;
    double real_value = 0.0;
    const char *path = "umicom-settings-test.conf";

    assert(umi_settings_create(TEST_SCHEMA,
                               sizeof(TEST_SCHEMA) / sizeof(TEST_SCHEMA[0]),
                               &settings) == UMI_STATUS_OK);
    assert(settings != NULL);
    assert(umi_settings_count(settings) == 4U);
    assert(umi_settings_has(settings, "test.count") != 0);
    assert(umi_settings_has(settings, "test.unknown") == 0);

    assert(umi_settings_get_text(settings,
                                 "test.name",
                                 text,
                                 sizeof(text)) == UMI_STATUS_OK);
    assert(strcmp(text, "Umicom") == 0);
    assert(umi_settings_get_boolean(settings,
                                    "test.enabled",
                                    &boolean_value) == UMI_STATUS_OK);
    assert(boolean_value == 1);
    assert(umi_settings_get_integer(settings,
                                    "test.count",
                                    &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 4);
    assert(umi_settings_get_real(settings,
                                 "test.ratio",
                                 &real_value) == UMI_STATUS_OK);
    assert(real_value > 0.499999 && real_value < 0.500001);

    assert(umi_settings_set_text(settings,
                                 "test.enabled",
                                 "off") == UMI_STATUS_OK);
    assert(umi_settings_get_boolean(settings,
                                    "test.enabled",
                                    &boolean_value) == UMI_STATUS_OK);
    assert(boolean_value == 0);

    assert(umi_settings_set_integer(settings,
                                    "test.count",
                                    12) == UMI_STATUS_OK);
    assert(umi_settings_set_integer(settings,
                                    "test.count",
                                    99) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_settings_set_real(settings,
                                 "test.ratio",
                                 0.75) == UMI_STATUS_OK);
    assert(umi_settings_set_real(settings,
                                 "test.ratio",
                                 2.0) == UMI_STATUS_INVALID_ARGUMENT);

    assert(umi_settings_definition_at(settings,
                                      2U,
                                      &definition) == UMI_STATUS_OK);
    assert(strcmp(definition.key, "test.count") == 0);
    assert(definition.type == UMI_SETTING_INTEGER);
    assert(definition.has_integer_range != 0);

    assert(umi_settings_save_file(settings, path) == UMI_STATUS_OK);
    assert(umi_settings_create(TEST_SCHEMA,
                               sizeof(TEST_SCHEMA) / sizeof(TEST_SCHEMA[0]),
                               &loaded) == UMI_STATUS_OK);
    assert(umi_settings_load_file(loaded, path) == UMI_STATUS_OK);
    assert(umi_settings_get_integer(loaded,
                                    "test.count",
                                    &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 12);
    assert(umi_settings_get_real(loaded,
                                 "test.ratio",
                                 &real_value) == UMI_STATUS_OK);
    assert(real_value > 0.749999 && real_value < 0.750001);

    assert(umi_settings_reset(loaded, "test.count") == UMI_STATUS_OK);
    assert(umi_settings_get_integer(loaded,
                                    "test.count",
                                    &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 4);
    umi_settings_reset_all(loaded);

    umi_settings_destroy(loaded);
    umi_settings_destroy(settings);
    (void)remove(path);
    return 0;
}
