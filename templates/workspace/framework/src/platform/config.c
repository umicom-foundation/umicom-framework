/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/platform/config.c
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

static char *trim(char *text)
{
    char *end;
    while (*text != '\0' && isspace((unsigned char)*text)) text++;
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) end--;
    *end = '\0';
    return text;
}

UmiStatus umi_config_create(UmiConfig **out_config)
{
    if (out_config == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_config = calloc(1U, sizeof(UmiConfig));
    return *out_config != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}
void umi_config_destroy(UmiConfig *config) { free(config); }
UmiStatus umi_config_set(UmiConfig *config, const char *key, const char *value)
{
    size_t i;
    if (config == 0 || key == 0 || value == 0) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < config->count; ++i) {
        if (strcmp(config->items[i].key, key) == 0) {
            (void)snprintf(config->items[i].value, sizeof(config->items[i].value), "%s", value);
            return UMI_STATUS_OK;
        }
    }
    if (config->count >= UMI_CONFIG_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(config->items[config->count].key, sizeof(config->items[config->count].key), "%s", key);
    (void)snprintf(config->items[config->count].value, sizeof(config->items[config->count].value), "%s", value);
    config->count++;
    return UMI_STATUS_OK;
}
const char *umi_config_get(const UmiConfig *config, const char *key, const char *fallback)
{
    size_t i;
    if (config == 0 || key == 0) return fallback;
    for (i = 0U; i < config->count; ++i)
        if (strcmp(config->items[i].key, key) == 0) return config->items[i].value;
    return fallback;
}
UmiStatus umi_config_load_file(UmiConfig *config, const char *path)
{
    FILE *file;
    char line[768];
    if (config == 0 || path == 0) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    if (file == 0) return UMI_STATUS_IO_ERROR;
    while (fgets(line, sizeof(line), file) != 0) {
        char *separator;
        char *key = trim(line);
        char *value;
        if (key[0] == '\0' || key[0] == '#') continue;
        separator = strchr(key, '=');
        if (separator == 0) { fclose(file); return UMI_STATUS_PARSE_ERROR; }
        *separator = '\0';
        value = trim(separator + 1);
        key = trim(key);
        if (umi_config_set(config, key, value) != UMI_STATUS_OK) {
            fclose(file); return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    fclose(file);
    return UMI_STATUS_OK;
}
size_t umi_config_count(const UmiConfig *config) { return config != 0 ? config->count : 0U; }
