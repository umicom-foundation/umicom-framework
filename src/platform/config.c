/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/config.c
 *
 * PURPOSE:
 *   Implement the config behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_CONFIG_MAX_ITEMS 256U

typedef struct UmiConfigItem { char key[128]; char value[512]; } UmiConfigItem;
struct UmiConfig { UmiConfigItem items[UMI_CONFIG_MAX_ITEMS]; size_t count; };

/* Provide the trim operation used by this module and its client applications. */
static char *trim(char *text)
{
    char *end;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0' && isspace((unsigned char)*text)) text++;
    end = text + strlen(text);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > text && isspace((unsigned char)end[-1])) end--;
    *end = '\0';
    return text;
}

/* Initialise config from caller-provided values so later operations receive a known state. */
UmiStatus umi_config_create(UmiConfig **out_config)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_config == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_config = calloc(1U, sizeof(UmiConfig));
    return *out_config != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}
/* Release or reset state held by config so the same storage can be reused safely. */
void umi_config_destroy(UmiConfig *config) { free(config); }
/* Copy config into module-owned storage so callers keep ownership of their input values. */
UmiStatus umi_config_set(UmiConfig *config, const char *key, const char *value)
{
    size_t i;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (config == 0 || key == 0 || value == 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < config->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(config->items[i].key, key) == 0) {
            (void)snprintf(config->items[i].value, sizeof(config->items[i].value), "%s", value);
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (config->count >= UMI_CONFIG_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(config->items[config->count].key, sizeof(config->items[config->count].key), "%s", key);
    (void)snprintf(config->items[config->count].value, sizeof(config->items[config->count].value), "%s", value);
    config->count++;
    return UMI_STATUS_OK;
}
/* Provide the config get operation used by this module and its client applications. */
const char *umi_config_get(const UmiConfig *config, const char *key, const char *fallback)
{
    size_t i;
    /* Apply this branch only when its contract condition is satisfied. */
    if (config == 0 || key == 0) return fallback;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < config->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(config->items[i].key, key) == 0) return config->items[i].value;
    return fallback;
}
/* Provide the config load file operation used by this module and its client applications. */
UmiStatus umi_config_load_file(UmiConfig *config, const char *path)
{
    FILE *file;
    char line[768];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (config == 0 || path == 0) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (file == 0) return UMI_STATUS_IO_ERROR;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (fgets(line, sizeof(line), file) != 0) {
        char *separator;
        char *key = trim(line);
        char *value;
        /* Apply this branch only when its contract condition is satisfied. */
        if (key[0] == '\0' || key[0] == '#') continue;
        separator = strchr(key, '=');
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (separator == 0) { fclose(file); return UMI_STATUS_PARSE_ERROR; }
        *separator = '\0';
        value = trim(separator + 1);
        key = trim(key);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_config_set(config, key, value) != UMI_STATUS_OK) {
            fclose(file); return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    fclose(file);
    return UMI_STATUS_OK;
}
/* Return the number of records represented by config without changing their state. */
size_t umi_config_count(const UmiConfig *config) { return config != 0 ? config->count : 0U; }
