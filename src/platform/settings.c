/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/settings.c
 *
 * PURPOSE:
 *   Implement the typed settings repository declared by settings.h.  Values
 *   are validated and normalised before storage, schema metadata is immutable,
 *   and a short C atomic lock protects concurrent readers and writers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/settings.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_SETTINGS_LINE_CAPACITY 1024U

typedef struct UmiSettingEntry {
    char key[UMI_SETTING_KEY_CAPACITY];
    UmiSettingType type;
    char default_value[UMI_SETTING_VALUE_CAPACITY];
    char value[UMI_SETTING_VALUE_CAPACITY];
    char description[UMI_SETTING_DESCRIPTION_CAPACITY];
    int64_t minimum_integer;
    int64_t maximum_integer;
    double minimum_real;
    double maximum_real;
    int has_integer_range;
    int has_real_range;
} UmiSettingEntry;

struct UmiSettings {
    UmiSettingEntry *entries;
    size_t count;
    atomic_flag lock;
};

static void settings_lock(const UmiSettings *settings)
{
    UmiSettings *mutable_settings = (UmiSettings *)settings;

    while (atomic_flag_test_and_set_explicit(&mutable_settings->lock,
                                              memory_order_acquire)) {
        /* Settings operations are deliberately short; wait for the owner. */
    }
}

static void settings_unlock(const UmiSettings *settings)
{
    UmiSettings *mutable_settings = (UmiSettings *)settings;
    atomic_flag_clear_explicit(&mutable_settings->lock, memory_order_release);
}

static UmiStatus copy_text(char *destination,
                           size_t destination_capacity,
                           const char *source)
{
    size_t source_length;

    if (destination == NULL || destination_capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    source_length = strlen(source);
    if (source_length >= destination_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, source_length + 1U);
    return UMI_STATUS_OK;
}

static char *trim(char *text)
{
    char *end;

    if (text == NULL) {
        return NULL;
    }

    while (*text != '\0' && isspace((unsigned char)*text) != 0) {
        ++text;
    }

    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1]) != 0) {
        --end;
    }
    *end = '\0';
    return text;
}

static int text_equal_ignore_case(const char *left, const char *right)
{
    if (left == NULL || right == NULL) {
        return 0;
    }

    while (*left != '\0' && *right != '\0') {
        if (tolower((unsigned char)*left) !=
            tolower((unsigned char)*right)) {
            return 0;
        }
        ++left;
        ++right;
    }

    return *left == '\0' && *right == '\0';
}

static int find_index(const UmiSettings *settings,
                      const char *key,
                      size_t *out_index)
{
    size_t index;

    if (settings == NULL || key == NULL || out_index == NULL) {
        return 0;
    }

    for (index = 0U; index < settings->count; ++index) {
        if (strcmp(settings->entries[index].key, key) == 0) {
            *out_index = index;
            return 1;
        }
    }
    return 0;
}

static UmiStatus normalise_boolean(const char *value,
                                   char *out_value,
                                   size_t out_capacity)
{
    int result;

    if (text_equal_ignore_case(value, "true") ||
        text_equal_ignore_case(value, "yes") ||
        text_equal_ignore_case(value, "on") ||
        strcmp(value, "1") == 0) {
        result = 1;
    } else if (text_equal_ignore_case(value, "false") ||
               text_equal_ignore_case(value, "no") ||
               text_equal_ignore_case(value, "off") ||
               strcmp(value, "0") == 0) {
        result = 0;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }

    return copy_text(out_value, out_capacity, result != 0 ? "true" : "false");
}

static UmiStatus normalise_integer(const UmiSettingEntry *entry,
                                   const char *value,
                                   char *out_value,
                                   size_t out_capacity)
{
    char *end = NULL;
    long long parsed;
    int written;

    errno = 0;
    parsed = strtoll(value, &end, 10);
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    if (entry->has_integer_range != 0 &&
        (parsed < entry->minimum_integer || parsed > entry->maximum_integer)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(out_value, out_capacity, "%lld", parsed);
    if (written < 0 || (size_t)written >= out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

static UmiStatus normalise_real(const UmiSettingEntry *entry,
                                const char *value,
                                char *out_value,
                                size_t out_capacity)
{
    char *end = NULL;
    double parsed;
    int written;

    errno = 0;
    parsed = strtod(value, &end);
    if (errno != 0 || end == value || end == NULL || *end != '\0' ||
        isfinite(parsed) == 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    if (entry->has_real_range != 0 &&
        (parsed < entry->minimum_real || parsed > entry->maximum_real)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(out_value, out_capacity, "%.17g", parsed);
    if (written < 0 || (size_t)written >= out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

static UmiStatus normalise_value(const UmiSettingEntry *entry,
                                 const char *value,
                                 char *out_value,
                                 size_t out_capacity)
{
    if (entry == NULL || value == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    switch (entry->type) {
        case UMI_SETTING_STRING:
            if (strchr(value, '\n') != NULL || strchr(value, '\r') != NULL) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            return copy_text(out_value, out_capacity, value);
        case UMI_SETTING_BOOLEAN:
            return normalise_boolean(value, out_value, out_capacity);
        case UMI_SETTING_INTEGER:
            return normalise_integer(entry, value, out_value, out_capacity);
        case UMI_SETTING_REAL:
            return normalise_real(entry, value, out_value, out_capacity);
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
}

UmiStatus umi_settings_create(const UmiSettingDefinition *definitions,
                              size_t definition_count,
                              UmiSettings **out_settings)
{
    UmiSettings *settings;
    size_t index;

    if (definitions == NULL || definition_count == 0U ||
        definition_count > UMI_SETTINGS_MAX_DEFINITIONS ||
        out_settings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_settings = NULL;

    settings = (UmiSettings *)calloc(1U, sizeof(*settings));
    if (settings == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    settings->entries = (UmiSettingEntry *)calloc(definition_count,
                                                   sizeof(*settings->entries));
    if (settings->entries == NULL) {
        free(settings);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    settings->count = definition_count;
    atomic_flag_clear_explicit(&settings->lock, memory_order_release);

    for (index = 0U; index < definition_count; ++index) {
        UmiSettingEntry *entry = &settings->entries[index];
        const UmiSettingDefinition *definition = &definitions[index];
        size_t previous;
        UmiStatus status;

        if (definition->key == NULL || definition->key[0] == '\0' ||
            definition->default_value == NULL) {
            umi_settings_destroy(settings);
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        for (previous = 0U; previous < index; ++previous) {
            if (strcmp(settings->entries[previous].key,
                       definition->key) == 0) {
                umi_settings_destroy(settings);
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }

        status = copy_text(entry->key,
                           sizeof(entry->key),
                           definition->key);
        if (status != UMI_STATUS_OK) {
            umi_settings_destroy(settings);
            return status;
        }

        entry->type = definition->type;
        entry->minimum_integer = definition->minimum_integer;
        entry->maximum_integer = definition->maximum_integer;
        entry->minimum_real = definition->minimum_real;
        entry->maximum_real = definition->maximum_real;
        entry->has_integer_range = definition->has_integer_range;
        entry->has_real_range = definition->has_real_range;

        status = copy_text(entry->description,
                           sizeof(entry->description),
                           definition->description != NULL
                               ? definition->description
                               : "");
        if (status != UMI_STATUS_OK) {
            umi_settings_destroy(settings);
            return status;
        }

        status = normalise_value(entry,
                                 definition->default_value,
                                 entry->default_value,
                                 sizeof(entry->default_value));
        if (status != UMI_STATUS_OK) {
            umi_settings_destroy(settings);
            return status;
        }
        status = copy_text(entry->value,
                           sizeof(entry->value),
                           entry->default_value);
        if (status != UMI_STATUS_OK) {
            umi_settings_destroy(settings);
            return status;
        }
    }

    *out_settings = settings;
    return UMI_STATUS_OK;
}

void umi_settings_destroy(UmiSettings *settings)
{
    if (settings == NULL) {
        return;
    }
    free(settings->entries);
    settings->entries = NULL;
    free(settings);
}

size_t umi_settings_count(const UmiSettings *settings)
{
    return settings != NULL ? settings->count : 0U;
}

int umi_settings_has(const UmiSettings *settings, const char *key)
{
    size_t index;
    return find_index(settings, key, &index);
}

UmiStatus umi_settings_definition_at(const UmiSettings *settings,
                                     size_t index,
                                     UmiSettingDefinitionView *out_definition)
{
    const UmiSettingEntry *entry;

    if (settings == NULL || out_definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= settings->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    entry = &settings->entries[index];
    out_definition->key = entry->key;
    out_definition->type = entry->type;
    out_definition->default_value = entry->default_value;
    out_definition->description = entry->description;
    out_definition->minimum_integer = entry->minimum_integer;
    out_definition->maximum_integer = entry->maximum_integer;
    out_definition->minimum_real = entry->minimum_real;
    out_definition->maximum_real = entry->maximum_real;
    out_definition->has_integer_range = entry->has_integer_range;
    out_definition->has_real_range = entry->has_real_range;
    return UMI_STATUS_OK;
}

UmiStatus umi_settings_set_text(UmiSettings *settings,
                                const char *key,
                                const char *value)
{
    size_t index;
    char normalised[UMI_SETTING_VALUE_CAPACITY];
    UmiStatus status;

    if (!find_index(settings, key, &index) || value == NULL) {
        return settings == NULL || key == NULL || value == NULL
            ? UMI_STATUS_INVALID_ARGUMENT
            : UMI_STATUS_NOT_FOUND;
    }

    status = normalise_value(&settings->entries[index],
                             value,
                             normalised,
                             sizeof(normalised));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    settings_lock(settings);
    status = copy_text(settings->entries[index].value,
                       sizeof(settings->entries[index].value),
                       normalised);
    settings_unlock(settings);
    return status;
}

UmiStatus umi_settings_get_text(const UmiSettings *settings,
                                const char *key,
                                char *out_value,
                                size_t out_value_capacity)
{
    size_t index;
    UmiStatus status;

    if (!find_index(settings, key, &index) || out_value == NULL ||
        out_value_capacity == 0U) {
        return settings == NULL || key == NULL || out_value == NULL ||
               out_value_capacity == 0U
            ? UMI_STATUS_INVALID_ARGUMENT
            : UMI_STATUS_NOT_FOUND;
    }

    settings_lock(settings);
    status = copy_text(out_value,
                       out_value_capacity,
                       settings->entries[index].value);
    settings_unlock(settings);
    return status;
}

UmiStatus umi_settings_set_boolean(UmiSettings *settings,
                                   const char *key,
                                   int value)
{
    return umi_settings_set_text(settings, key, value != 0 ? "true" : "false");
}

UmiStatus umi_settings_get_boolean(const UmiSettings *settings,
                                   const char *key,
                                   int *out_value)
{
    char value[UMI_SETTING_VALUE_CAPACITY];
    UmiStatus status;

    if (out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_settings_get_text(settings, key, value, sizeof(value));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    *out_value = strcmp(value, "true") == 0 ? 1 : 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_settings_set_integer(UmiSettings *settings,
                                   const char *key,
                                   int64_t value)
{
    char text[64];
    int written = snprintf(text, sizeof(text), "%lld", (long long)value);

    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_settings_set_text(settings, key, text);
}

UmiStatus umi_settings_get_integer(const UmiSettings *settings,
                                   const char *key,
                                   int64_t *out_value)
{
    char value[UMI_SETTING_VALUE_CAPACITY];
    char *end = NULL;
    long long parsed;
    UmiStatus status;

    if (out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_settings_get_text(settings, key, value, sizeof(value));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    errno = 0;
    parsed = strtoll(value, &end, 10);
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = (int64_t)parsed;
    return UMI_STATUS_OK;
}

UmiStatus umi_settings_set_real(UmiSettings *settings,
                                const char *key,
                                double value)
{
    char text[64];
    int written;

    if (isfinite(value) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(text, sizeof(text), "%.17g", value);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_settings_set_text(settings, key, text);
}

UmiStatus umi_settings_get_real(const UmiSettings *settings,
                                const char *key,
                                double *out_value)
{
    char value[UMI_SETTING_VALUE_CAPACITY];
    char *end = NULL;
    double parsed;
    UmiStatus status;

    if (out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_settings_get_text(settings, key, value, sizeof(value));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    errno = 0;
    parsed = strtod(value, &end);
    if (errno != 0 || end == value || end == NULL || *end != '\0' ||
        isfinite(parsed) == 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = parsed;
    return UMI_STATUS_OK;
}

UmiStatus umi_settings_reset(UmiSettings *settings, const char *key)
{
    size_t index;
    UmiStatus status;

    if (!find_index(settings, key, &index)) {
        return settings == NULL || key == NULL
            ? UMI_STATUS_INVALID_ARGUMENT
            : UMI_STATUS_NOT_FOUND;
    }

    settings_lock(settings);
    status = copy_text(settings->entries[index].value,
                       sizeof(settings->entries[index].value),
                       settings->entries[index].default_value);
    settings_unlock(settings);
    return status;
}

void umi_settings_reset_all(UmiSettings *settings)
{
    size_t index;

    if (settings == NULL) {
        return;
    }

    settings_lock(settings);
    for (index = 0U; index < settings->count; ++index) {
        (void)copy_text(settings->entries[index].value,
                        sizeof(settings->entries[index].value),
                        settings->entries[index].default_value);
    }
    settings_unlock(settings);
}

UmiStatus umi_settings_load_file(UmiSettings *settings, const char *path)
{
    FILE *file;
    char line[UMI_SETTINGS_LINE_CAPACITY];
    size_t line_number = 0U;

    if (settings == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, "rb");
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *separator;
        char *key;
        char *value;
        UmiStatus status;

        ++line_number;
        key = trim(line);
        if (key == NULL || key[0] == '\0' || key[0] == '#' || key[0] == ';') {
            continue;
        }

        separator = strchr(key, '=');
        if (separator == NULL) {
            (void)fclose(file);
            (void)line_number;
            return UMI_STATUS_PARSE_ERROR;
        }
        *separator = '\0';
        value = trim(separator + 1);
        key = trim(key);

        if (!umi_settings_has(settings, key)) {
            /* Forward-compatible behaviour: ignore settings unknown to this build. */
            continue;
        }

        status = umi_settings_set_text(settings, key, value);
        if (status != UMI_STATUS_OK) {
            (void)fclose(file);
            return status;
        }
    }

    if (ferror(file) != 0) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    (void)fclose(file);
    return UMI_STATUS_OK;
}

UmiStatus umi_settings_save_file(const UmiSettings *settings, const char *path)
{
    FILE *file;
    size_t index;

    if (settings == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, "wb");
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    if (fprintf(file,
                "# Umicom typed settings\n"
                "# Generated by Umicom Framework.\n\n") < 0) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }

    settings_lock(settings);
    for (index = 0U; index < settings->count; ++index) {
        const UmiSettingEntry *entry = &settings->entries[index];

        if (entry->description[0] != '\0' &&
            fprintf(file, "# %s\n", entry->description) < 0) {
            settings_unlock(settings);
            (void)fclose(file);
            return UMI_STATUS_IO_ERROR;
        }
        if (fprintf(file,
                    "# type: %s\n%s=%s\n\n",
                    umi_setting_type_text(entry->type),
                    entry->key,
                    entry->value) < 0) {
            settings_unlock(settings);
            (void)fclose(file);
            return UMI_STATUS_IO_ERROR;
        }
    }
    settings_unlock(settings);

    if (fclose(file) != 0) {
        return UMI_STATUS_IO_ERROR;
    }
    return UMI_STATUS_OK;
}

const char *umi_setting_type_text(UmiSettingType type)
{
    switch (type) {
        case UMI_SETTING_STRING: return "string";
        case UMI_SETTING_BOOLEAN: return "boolean";
        case UMI_SETTING_INTEGER: return "integer";
        case UMI_SETTING_REAL: return "real";
        default: return "unknown";
    }
}
