/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/path.c
 *
 * PURPOSE:
 *   Validate and normalize repository-relative paths portably.
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
#include "umicom/repository/path.h"

#include <ctype.h>
#include <string.h>

/* Provide the drive prefix operation used by this module and its client applications. */
static int drive_prefix(const char *path)
{
    return path != NULL &&
        isalpha((unsigned char)path[0]) != 0 &&
        path[1] == ':';
}

/*
 * Provide the repository control path is safe relative operation used by this module and
 * its client applications.
 */
int umi_repository_control_path_is_safe_relative(const char *path)
{
    const char *segment;
    const char *cursor;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' || path[0] == '/' ||
        path[0] == '\\' || drive_prefix(path)) {
        return 0;
    }
    segment = path;
    cursor = path;
    /* Visit each bounded item once so every record receives the same rule. */
    for (;;) {
        unsigned char ch = (unsigned char)*cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '/' || *cursor == '\\' || *cursor == '\0') {
            length = (size_t)(cursor - segment);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (length == 0U ||
                (length == 1U && segment[0] == '.') ||
                (length == 2U && segment[0] == '.' && segment[1] == '.')) {
                return 0;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (*cursor == '\0') break;
            segment = cursor + 1;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (iscntrl(ch) != 0 || *cursor == ':') {
            return 0;
        }
        ++cursor;
    }
    return 1;
}

/*
 * Provide the repository control path normalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_control_path_normalize(
    const char *path, char *out_path, size_t capacity)
{
    size_t length;
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_repository_control_path_is_safe_relative(path) ||
        out_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        out_path[index] = path[index] == '\\' ? '/' : path[index];
    }
    out_path[length] = '\0';
    return UMI_STATUS_OK;
}
