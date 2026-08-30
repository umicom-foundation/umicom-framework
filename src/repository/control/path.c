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

static int drive_prefix(const char *path)
{
    return path != NULL &&
        isalpha((unsigned char)path[0]) != 0 &&
        path[1] == ':';
}

int umi_repository_control_path_is_safe_relative(const char *path)
{
    const char *segment;
    const char *cursor;
    size_t length;
    if (path == NULL || path[0] == '\0' || path[0] == '/' ||
        path[0] == '\\' || drive_prefix(path)) {
        return 0;
    }
    segment = path;
    cursor = path;
    for (;;) {
        unsigned char ch = (unsigned char)*cursor;
        if (*cursor == '/' || *cursor == '\\' || *cursor == '\0') {
            length = (size_t)(cursor - segment);
            if (length == 0U ||
                (length == 1U && segment[0] == '.') ||
                (length == 2U && segment[0] == '.' && segment[1] == '.')) {
                return 0;
            }
            if (*cursor == '\0') break;
            segment = cursor + 1;
        } else if (iscntrl(ch) != 0 || *cursor == ':') {
            return 0;
        }
        ++cursor;
    }
    return 1;
}

UmiStatus umi_repository_control_path_normalize(
    const char *path, char *out_path, size_t capacity)
{
    size_t length;
    size_t index;
    if (!umi_repository_control_path_is_safe_relative(path) ||
        out_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(path);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < length; ++index) {
        out_path[index] = path[index] == '\\' ? '/' : path[index];
    }
    out_path[length] = '\0';
    return UMI_STATUS_OK;
}
