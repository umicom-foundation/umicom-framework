/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/path.c
 *
 * PURPOSE:
 *   Implement portable, bounded workspace path normalisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/path.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int is_absolute_path(const char *path)
{
    if (path == NULL || path[0] == '\0') return 0;

    if (path[0] == '/' || path[0] == '\\') return 1;

    if (isalpha((unsigned char)path[0]) &&
        path[1] == ':' &&
        (path[2] == '/' || path[2] == '\\')) {
        return 1;
    }

    return 0;
}

UmiStatus umi_ai_coding_runtime_path_normalize_relative(
    const char *path,
    char *out_path,
    size_t capacity)
{
    const char *cursor;
    size_t used = 0U;
    size_t segment_start = 0U;

    if (path == NULL || path[0] == '\0' ||
        out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (is_absolute_path(path)) return UMI_STATUS_PERMISSION_DENIED;

    cursor = path;

    while (*cursor != '\0') {
        char value = *cursor++;

        if (value == '\\') value = '/';

        if (value == '/') {
            size_t segment_length = used - segment_start;

            if (segment_length == 0U) {
                continue;
            }

            if (segment_length == 1U &&
                out_path[segment_start] == '.') {
                used = segment_start;
                continue;
            }

            if (segment_length == 2U &&
                out_path[segment_start] == '.' &&
                out_path[segment_start + 1U] == '.') {
                return UMI_STATUS_PERMISSION_DENIED;
            }

            if (used + 1U >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            out_path[used++] = '/';
            segment_start = used;
            continue;
        }

        if ((unsigned char)value < 0x20U) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        if (used + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        out_path[used++] = value;
    }

    if (used == segment_start) {
        if (used > 0U && out_path[used - 1U] == '/') --used;
    } else {
        size_t segment_length = used - segment_start;

        if (segment_length == 1U &&
            out_path[segment_start] == '.') {
            used = segment_start;
            if (used > 0U && out_path[used - 1U] == '/') --used;
        } else if (segment_length == 2U &&
                   out_path[segment_start] == '.' &&
                   out_path[segment_start + 1U] == '.') {
            return UMI_STATUS_PERMISSION_DENIED;
        }
    }

    if (used == 0U) return UMI_STATUS_INVALID_ARGUMENT;

    out_path[used] = '\0';

    if (!umi_ai_coding_path_is_safe_relative(out_path)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_runtime_path_join(
    const char *root,
    const char *relative_path,
    char *out_path,
    size_t capacity)
{
    char normalized[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    size_t root_length;
    int written;
    UmiStatus status;

    if (root == NULL || root[0] == '\0' ||
        relative_path == NULL ||
        out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_runtime_path_normalize_relative(
        relative_path,
        normalized,
        sizeof(normalized));
    if (status != UMI_STATUS_OK) return status;

    root_length = strlen(root);

    written = snprintf(
        out_path,
        capacity,
        "%s%s%s",
        root,
        root_length > 0U &&
                (root[root_length - 1U] == '/' ||
                 root[root_length - 1U] == '\\')
            ? ""
            : "/",
        normalized);

    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

const char *umi_ai_coding_runtime_path_extension(const char *path)
{
    const char *cursor;
    const char *extension = NULL;

    if (path == NULL) return NULL;

    for (cursor = path; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') {
            extension = NULL;
        } else if (*cursor == '.') {
            extension = cursor;
        }
    }

    return extension;
}
