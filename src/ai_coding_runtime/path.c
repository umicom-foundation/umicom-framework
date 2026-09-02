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

/* Provide the is absolute path operation used by this module and its client applications. */
static int is_absolute_path(const char *path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') return 0;

    /* Apply this branch only when its contract condition is satisfied. */
    if (path[0] == '/' || path[0] == '\\') return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (isalpha((unsigned char)path[0]) &&
        path[1] == ':' &&
        (path[2] == '/' || path[2] == '\\')) {
        return 1;
    }

    return 0;
}

/*
 * Provide the ai coding runtime path normalize relative operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_runtime_path_normalize_relative(
    const char *path,
    char *out_path,
    size_t capacity)
{
    const char *cursor;
    size_t used = 0U;
    size_t segment_start = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' ||
        out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (is_absolute_path(path)) return UMI_STATUS_PERMISSION_DENIED;

    cursor = path;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        char value = *cursor++;

        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\\') value = '/';

        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '/') {
            size_t segment_length = used - segment_start;

            /* Apply this branch only when its contract condition is satisfied. */
            if (segment_length == 0U) {
                continue;
            }

            /* Apply this branch only when its contract condition is satisfied. */
            if (segment_length == 1U &&
                out_path[segment_start] == '.') {
                used = segment_start;
                continue;
            }

            /* Apply this branch only when its contract condition is satisfied. */
            if (segment_length == 2U &&
                out_path[segment_start] == '.' &&
                out_path[segment_start + 1U] == '.') {
                return UMI_STATUS_PERMISSION_DENIED;
            }

            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (used + 1U >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            out_path[used++] = '/';
            segment_start = used;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if ((unsigned char)value < 0x20U) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (used + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        out_path[used++] = value;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (used == segment_start) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (used > 0U && out_path[used - 1U] == '/') --used;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        size_t segment_length = used - segment_start;

        /* Apply this branch only when its contract condition is satisfied. */
        if (segment_length == 1U &&
            out_path[segment_start] == '.') {
            used = segment_start;
            /* Apply this branch only when its contract condition is satisfied. */
            if (used > 0U && out_path[used - 1U] == '/') --used;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (segment_length == 2U &&
                   out_path[segment_start] == '.' &&
                   out_path[segment_start + 1U] == '.') {
            return UMI_STATUS_PERMISSION_DENIED;
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (used == 0U) return UMI_STATUS_INVALID_ARGUMENT;

    out_path[used] = '\0';

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ai_coding_path_is_safe_relative(out_path)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding runtime path join operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || root[0] == '\0' ||
        relative_path == NULL ||
        out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_runtime_path_normalize_relative(
        relative_path,
        normalized,
        sizeof(normalized));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the ai coding runtime path extension operation used by this module and its
 * client applications.
 */
const char *umi_ai_coding_runtime_path_extension(const char *path)
{
    const char *cursor;
    const char *extension = NULL;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = path; *cursor != '\0'; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '/' || *cursor == '\\') {
            extension = NULL;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '.') {
            extension = cursor;
        }
    }

    return extension;
}
