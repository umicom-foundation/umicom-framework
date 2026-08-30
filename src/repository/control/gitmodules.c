/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/gitmodules.c
 *
 * PURPOSE:
 *   Parse .gitmodules text into the canonical submodule inventory.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/gitmodules.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *trim_left(char *text)
{
    while (text != NULL && *text != '\0' &&
           isspace((unsigned char)*text) != 0) {
        ++text;
    }
    return text;
}

static void trim_right(char *text)
{
    size_t length;
    if (text == NULL) return;
    length = strlen(text);
    while (length > 0U &&
           isspace((unsigned char)text[length - 1U]) != 0) {
        text[--length] = '\0';
    }
}

static UmiStatus copy_text(
    char *out, size_t capacity, const char *text)
{
    size_t length = strlen(text);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus flush_submodule(
    UmiRepositoryInventory *inventory,
    const char *name,
    const char *path,
    const char *url,
    const char *branch)
{
    UmiRepositorySubmodule submodule;
    if (name[0] == '\0') return UMI_STATUS_OK;
    if (path[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    if (umi_repository_submodule_init(
            &submodule, name, path, url, branch, 1) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    return umi_repository_inventory_add(inventory, &submodule);
}

UmiStatus umi_repository_gitmodules_parse(
    const char *text,
    UmiRepositoryInventory *out_inventory)
{
    char *copy;
    char *cursor;
    char name[UMI_REPOSITORY_CONTROL_NAME_CAPACITY] = {0};
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY] = {0};
    char url[UMI_REPOSITORY_CONTROL_URL_CAPACITY] = {0};
    char branch[UMI_REPOSITORY_CONTROL_REF_CAPACITY] = {0};
    UmiStatus status = UMI_STATUS_OK;
    size_t length;

    if (text == NULL || out_inventory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_repository_inventory_init(out_inventory);
    length = strlen(text);
    copy = (char *)malloc(length + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(copy, text, length + 1U);
    cursor = copy;

    while (cursor != NULL && *cursor != '\0') {
        char *line = cursor;
        char *newline = strchr(cursor, '\n');
        char *item;
        if (newline != NULL) {
            *newline = '\0';
            cursor = newline + 1;
        } else {
            cursor = NULL;
        }
        item = trim_left(line);
        trim_right(item);
        if (item[0] == '\0' || item[0] == '#' || item[0] == ';') continue;

        if (item[0] == '[') {
            const char *first;
            const char *second;
            status = flush_submodule(
                out_inventory, name, path, url, branch);
            if (status != UMI_STATUS_OK) break;
            name[0] = path[0] = url[0] = branch[0] = '\0';
            if (strncmp(item, "[submodule", 10U) != 0) {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            first = strchr(item, '"');
            second = first != NULL ? strchr(first + 1, '"') : NULL;
            if (first == NULL || second == NULL || second == first + 1) {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            if ((size_t)(second - first - 1) >= sizeof(name)) {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            (void)memcpy(name, first + 1, (size_t)(second - first - 1));
            name[(size_t)(second - first - 1)] = '\0';
            continue;
        }

        {
            char *equals = strchr(item, '=');
            char *key;
            char *value;
            if (equals == NULL || name[0] == '\0') {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            *equals = '\0';
            key = trim_left(item);
            trim_right(key);
            value = trim_left(equals + 1);
            trim_right(value);
            if (strcmp(key, "path") == 0) {
                status = copy_text(path, sizeof(path), value);
            } else if (strcmp(key, "url") == 0) {
                status = copy_text(url, sizeof(url), value);
            } else if (strcmp(key, "branch") == 0) {
                status = copy_text(branch, sizeof(branch), value);
            }
            if (status != UMI_STATUS_OK) break;
        }
    }

    if (status == UMI_STATUS_OK) {
        status = flush_submodule(out_inventory, name, path, url, branch);
    }
    free(copy);
    if (status != UMI_STATUS_OK) return status;
    return out_inventory->count > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
