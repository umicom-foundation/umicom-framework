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

/* Provide the trim left operation used by this module and its client applications. */
static char *trim_left(char *text)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (text != NULL && *text != '\0' &&
           isspace((unsigned char)*text) != 0) {
        ++text;
    }
    return text;
}

/* Provide the trim right operation used by this module and its client applications. */
static void trim_right(char *text)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return;
    length = strlen(text);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length > 0U &&
           isspace((unsigned char)text[length - 1U]) != 0) {
        text[--length] = '\0';
    }
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(
    char *out, size_t capacity, const char *text)
{
    size_t length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the flush submodule operation used by this module and its client applications. */
static UmiStatus flush_submodule(
    UmiRepositoryInventory *inventory,
    const char *name,
    const char *path,
    const char *url,
    const char *branch)
{
    UmiRepositorySubmodule submodule;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (name[0] == '\0') return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (path[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repository_submodule_init(
            &submodule, name, path, url, branch, 1) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    return umi_repository_inventory_add(inventory, &submodule);
}

/*
 * Read repository gitmodules into validated module state and return a status when input
 * cannot be used.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_inventory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_repository_inventory_init(out_inventory);
    length = strlen(text);
    copy = (char *)malloc(length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(copy, text, length + 1U);
    cursor = copy;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && *cursor != '\0') {
        char *line = cursor;
        char *newline = strchr(cursor, '\n');
        char *item;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (newline != NULL) {
            *newline = '\0';
            cursor = newline + 1;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            cursor = NULL;
        }
        item = trim_left(line);
        trim_right(item);
        /* Apply this branch only when its contract condition is satisfied. */
        if (item[0] == '\0' || item[0] == '#' || item[0] == ';') continue;

        /* Apply this branch only when its contract condition is satisfied. */
        if (item[0] == '[') {
            const char *first;
            const char *second;
            status = flush_submodule(
                out_inventory, name, path, url, branch);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
            name[0] = path[0] = url[0] = branch[0] = '\0';
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strncmp(item, "[submodule", 10U) != 0) {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            first = strchr(item, '"');
            second = first != NULL ? strchr(first + 1, '"') : NULL;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (first == NULL || second == NULL || second == first + 1) {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (equals == NULL || name[0] == '\0') {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            *equals = '\0';
            key = trim_left(item);
            trim_right(key);
            value = trim_left(equals + 1);
            trim_right(value);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(key, "path") == 0) {
                status = copy_text(path, sizeof(path), value);
            } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "url") == 0) {
                status = copy_text(url, sizeof(url), value);
            } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "branch") == 0) {
                status = copy_text(branch, sizeof(branch), value);
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = flush_submodule(out_inventory, name, path, url, branch);
    }
    free(copy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return out_inventory->count > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
