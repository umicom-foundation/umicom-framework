/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/settings.c
 *
 * PURPOSE:
 *   Implement the typed settings repository declared by settings.h.  Values
 *   are validated and normalised before storage, schema metadata is immutable,
 *   and a short C atomic lock protects concurrent readers and writers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the settings lock operation used by this module and its client applications. */
static void settings_lock(const UmiSettings *settings)
{
    UmiSettings *mutable_settings = (UmiSettings *)settings;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (atomic_flag_test_and_set_explicit(&mutable_settings->lock,
                                              memory_order_acquire)) {
        /* Settings operations are deliberately short; wait for the owner. */
    }
}

/* Provide the settings unlock operation used by this module and its client applications. */
static void settings_unlock(const UmiSettings *settings)
{
    UmiSettings *mutable_settings = (UmiSettings *)settings;
    atomic_flag_clear_explicit(&mutable_settings->lock, memory_order_release);
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t destination_capacity,
                           const char *source)
{
    size_t source_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || destination_capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    source_length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (source_length >= destination_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, source_length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the trim operation used by this module and its client applications. */
static char *trim(char *text)
{
    char *end;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) {
        return NULL;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0' && isspace((unsigned char)*text) != 0) {
        ++text;
    }

    end = text + strlen(text);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > text && isspace((unsigned char)end[-1]) != 0) {
        --end;
    }
    *end = '\0';
    return text;
}

/*
 * Provide the text equal ignore case operation used by this module and its client
 * applications.
 */
static int text_equal_ignore_case(const char *left, const char *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) {
        return 0;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*left != '\0' && *right != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (tolower((unsigned char)*left) !=
            tolower((unsigned char)*right)) {
            return 0;
        }
        ++left;
        ++right;
    }

    return *left == '\0' && *right == '\0';
}

/* Provide the find index operation used by this module and its client applications. */
static int find_index(const UmiSettings *settings,
                      const char *key,
                      size_t *out_index)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings == NULL || key == NULL || out_index == NULL) {
        return 0;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < settings->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(settings->entries[index].key, key) == 0) {
            *out_index = index;
            return 1;
        }
    }
    return 0;
}

/* Provide the normalise boolean operation used by this module and its client applications. */
static UmiStatus normalise_boolean(const char *value,
                                   char *out_value,
                                   size_t out_capacity)
{
    int result;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (text_equal_ignore_case(value, "true") ||
        text_equal_ignore_case(value, "yes") ||
        text_equal_ignore_case(value, "on") ||
        strcmp(value, "1") == 0) {
        result = 1;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (text_equal_ignore_case(value, "false") ||
               text_equal_ignore_case(value, "no") ||
               text_equal_ignore_case(value, "off") ||
               strcmp(value, "0") == 0) {
        result = 0;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_PARSE_ERROR;
    }

    return copy_text(out_value, out_capacity, result != 0 ? "true" : "false");
}

/* Provide the normalise integer operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (entry->has_integer_range != 0 &&
        (parsed < entry->minimum_integer || parsed > entry->maximum_integer)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(out_value, out_capacity, "%lld", parsed);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Provide the normalise real operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (errno != 0 || end == value || end == NULL || *end != '\0' ||
        isfinite(parsed) == 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (entry->has_real_range != 0 &&
        (parsed < entry->minimum_real || parsed > entry->maximum_real)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(out_value, out_capacity, "%.17g", parsed);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= out_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Provide the normalise value operation used by this module and its client applications. */
static UmiStatus normalise_value(const UmiSettingEntry *entry,
                                 const char *value,
                                 char *out_value,
                                 size_t out_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || value == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (entry->type) {
        case UMI_SETTING_STRING:
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
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

/*
 * Initialise settings from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_settings_create(const UmiSettingDefinition *definitions,
                              size_t definition_count,
                              UmiSettings **out_settings)
{
    UmiSettings *settings;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definitions == NULL || definition_count == 0U ||
        definition_count > UMI_SETTINGS_MAX_DEFINITIONS ||
        out_settings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_settings = NULL;

    settings = (UmiSettings *)calloc(1U, sizeof(*settings));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    settings->entries = (UmiSettingEntry *)calloc(definition_count,
                                                   sizeof(*settings->entries));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings->entries == NULL) {
        free(settings);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    settings->count = definition_count;
    atomic_flag_clear_explicit(&settings->lock, memory_order_release);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition_count; ++index) {
        UmiSettingEntry *entry = &settings->entries[index];
        const UmiSettingDefinition *definition = &definitions[index];
        size_t previous;
        UmiStatus status;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (definition->key == NULL || definition->key[0] == '\0' ||
            definition->default_value == NULL) {
            umi_settings_destroy(settings);
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(settings->entries[previous].key,
                       definition->key) == 0) {
                umi_settings_destroy(settings);
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }

        status = copy_text(entry->key,
                           sizeof(entry->key),
                           definition->key);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_settings_destroy(settings);
            return status;
        }

        status = normalise_value(entry,
                                 definition->default_value,
                                 entry->default_value,
                                 sizeof(entry->default_value));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_settings_destroy(settings);
            return status;
        }
        status = copy_text(entry->value,
                           sizeof(entry->value),
                           entry->default_value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_settings_destroy(settings);
            return status;
        }
    }

    *out_settings = settings;
    return UMI_STATUS_OK;
}

/* Release or reset state held by settings so the same storage can be reused safely. */
void umi_settings_destroy(UmiSettings *settings)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings == NULL) {
        return;
    }
    free(settings->entries);
    settings->entries = NULL;
    free(settings);
}

/* Return the number of records represented by settings without changing their state. */
size_t umi_settings_count(const UmiSettings *settings)
{
    return settings != NULL ? settings->count : 0U;
}

/* Provide the settings has operation used by this module and its client applications. */
int umi_settings_has(const UmiSettings *settings, const char *key)
{
    size_t index;
    return find_index(settings, key, &index);
}

/*
 * Find settings definition while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_settings_definition_at(const UmiSettings *settings,
                                     size_t index,
                                     UmiSettingDefinitionView *out_definition)
{
    const UmiSettingEntry *entry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings == NULL || out_definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the settings set text operation used by this module and its client applications. */
UmiStatus umi_settings_set_text(UmiSettings *settings,
                                const char *key,
                                const char *value)
{
    size_t index;
    char normalised[UMI_SETTING_VALUE_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!find_index(settings, key, &index) || value == NULL) {
        return settings == NULL || key == NULL || value == NULL
            ? UMI_STATUS_INVALID_ARGUMENT
            : UMI_STATUS_NOT_FOUND;
    }

    status = normalise_value(&settings->entries[index],
                             value,
                             normalised,
                             sizeof(normalised));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/* Provide the settings get text operation used by this module and its client applications. */
UmiStatus umi_settings_get_text(const UmiSettings *settings,
                                const char *key,
                                char *out_value,
                                size_t out_value_capacity)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the settings set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_set_boolean(UmiSettings *settings,
                                   const char *key,
                                   int value)
{
    return umi_settings_set_text(settings, key, value != 0 ? "true" : "false");
}

/*
 * Provide the settings get boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_get_boolean(const UmiSettings *settings,
                                   const char *key,
                                   int *out_value)
{
    char value[UMI_SETTING_VALUE_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_settings_get_text(settings, key, value, sizeof(value));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    *out_value = strcmp(value, "true") == 0 ? 1 : 0;
    return UMI_STATUS_OK;
}

/*
 * Provide the settings set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_set_integer(UmiSettings *settings,
                                   const char *key,
                                   int64_t value)
{
    char text[64];
    int written = snprintf(text, sizeof(text), "%lld", (long long)value);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_settings_set_text(settings, key, text);
}

/*
 * Provide the settings get integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_get_integer(const UmiSettings *settings,
                                   const char *key,
                                   int64_t *out_value)
{
    char value[UMI_SETTING_VALUE_CAPACITY];
    char *end = NULL;
    long long parsed;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_settings_get_text(settings, key, value, sizeof(value));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    errno = 0;
    parsed = strtoll(value, &end, 10);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = (int64_t)parsed;
    return UMI_STATUS_OK;
}

/* Provide the settings set real operation used by this module and its client applications. */
UmiStatus umi_settings_set_real(UmiSettings *settings,
                                const char *key,
                                double value)
{
    char text[64];
    int written;

    /* Apply this branch only when its contract condition is satisfied. */
    if (isfinite(value) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(text, sizeof(text), "%.17g", value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_settings_set_text(settings, key, text);
}

/* Provide the settings get real operation used by this module and its client applications. */
UmiStatus umi_settings_get_real(const UmiSettings *settings,
                                const char *key,
                                double *out_value)
{
    char value[UMI_SETTING_VALUE_CAPACITY];
    char *end = NULL;
    double parsed;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_settings_get_text(settings, key, value, sizeof(value));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    errno = 0;
    parsed = strtod(value, &end);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (errno != 0 || end == value || end == NULL || *end != '\0' ||
        isfinite(parsed) == 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = parsed;
    return UMI_STATUS_OK;
}

/* Release or reset state held by settings so the same storage can be reused safely. */
UmiStatus umi_settings_reset(UmiSettings *settings, const char *key)
{
    size_t index;
    UmiStatus status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the settings reset all operation used by this module and its client
 * applications.
 */
void umi_settings_reset_all(UmiSettings *settings)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings == NULL) {
        return;
    }

    settings_lock(settings);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < settings->count; ++index) {
        (void)copy_text(settings->entries[index].value,
                        sizeof(settings->entries[index].value),
                        settings->entries[index].default_value);
    }
    settings_unlock(settings);
}

/*
 * Provide the settings load file operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_load_file(UmiSettings *settings, const char *path)
{
    FILE *file;
    char line[UMI_SETTINGS_LINE_CAPACITY];
    size_t line_number = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (fgets(line, sizeof(line), file) != NULL) {
        char *separator;
        char *key;
        char *value;
        UmiStatus status;

        ++line_number;
        key = trim(line);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (key == NULL || key[0] == '\0' || key[0] == '#' || key[0] == ';') {
            continue;
        }

        separator = strchr(key, '=');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (separator == NULL) {
            (void)fclose(file);
            (void)line_number;
            return UMI_STATUS_PARSE_ERROR;
        }
        *separator = '\0';
        value = trim(separator + 1);
        key = trim(key);

        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_settings_has(settings, key)) {
            /* Forward-compatible behaviour: ignore settings unknown to this build. */
            continue;
        }

        status = umi_settings_set_text(settings, key, value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)fclose(file);
            return status;
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (ferror(file) != 0) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    (void)fclose(file);
    return UMI_STATUS_OK;
}

/*
 * Provide the settings save file operation used by this module and its client
 * applications.
 */
UmiStatus umi_settings_save_file(const UmiSettings *settings, const char *path)
{
    FILE *file;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (fprintf(file,
                "# Umicom typed settings\n"
                "# Generated by Umicom Framework.\n\n") < 0) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }

    settings_lock(settings);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < settings->count; ++index) {
        const UmiSettingEntry *entry = &settings->entries[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->description[0] != '\0' &&
            fprintf(file, "# %s\n", entry->description) < 0) {
            settings_unlock(settings);
            (void)fclose(file);
            return UMI_STATUS_IO_ERROR;
        }
        /* Apply this branch only when its contract condition is satisfied. */
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

    /* Apply this branch only when its contract condition is satisfied. */
    if (fclose(file) != 0) {
        return UMI_STATUS_IO_ERROR;
    }
    return UMI_STATUS_OK;
}

/* Provide the setting type text operation used by this module and its client applications. */
const char *umi_setting_type_text(UmiSettingType type)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (type) {
        case UMI_SETTING_STRING: return "string";
        case UMI_SETTING_BOOLEAN: return "boolean";
        case UMI_SETTING_INTEGER: return "integer";
        case UMI_SETTING_REAL: return "real";
        default: return "unknown";
    }
}
