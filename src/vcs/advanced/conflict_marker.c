/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/conflict_marker.c
 *
 * PURPOSE:
 *   Implement textual merge-marker classification and balance validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/conflict_marker.h"

#include <string.h>

UmiVcsAdvancedConflictMarkerKind umi_vcs_advanced_conflict_marker_classify(
    const char *line)
{
    if (line == NULL) {
        return UMI_VCS_MARKER_NONE;
    }
    if (strncmp(line, "<<<<<<<", 7U) == 0) {
        return UMI_VCS_MARKER_OURS;
    }
    if (strncmp(line, "|||||||", 7U) == 0) {
        return UMI_VCS_MARKER_BASE;
    }
    if (strncmp(line, "=======", 7U) == 0) {
        return UMI_VCS_MARKER_SEPARATOR;
    }
    if (strncmp(line, ">>>>>>>", 7U) == 0) {
        return UMI_VCS_MARKER_THEIRS;
    }
    return UMI_VCS_MARKER_NONE;
}

int umi_vcs_advanced_conflict_marker_balanced(
    const char *text,
    size_t *out_conflicts)
{
    const char *cursor;
    const char *end;
    size_t conflicts = 0U;
    int state = 0;
    char line[256];

    if (text == NULL) {
        return 0;
    }

    cursor = text;
    while (*cursor != '\0') {
        size_t length;
        UmiVcsAdvancedConflictMarkerKind kind;

        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        kind = umi_vcs_advanced_conflict_marker_classify(line);

        if (kind == UMI_VCS_MARKER_OURS) {
            if (state != 0) {
                return 0;
            }
            state = 1;
            conflicts += 1U;
        } else if (kind == UMI_VCS_MARKER_SEPARATOR) {
            if (state != 1 && state != 2) {
                return 0;
            }
            state = 3;
        } else if (kind == UMI_VCS_MARKER_BASE) {
            if (state != 1) {
                return 0;
            }
            state = 2;
        } else if (kind == UMI_VCS_MARKER_THEIRS) {
            if (state != 3) {
                return 0;
            }
            state = 0;
        }

        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }

    if (out_conflicts != NULL) {
        *out_conflicts = conflicts;
    }
    return state == 0;
}
