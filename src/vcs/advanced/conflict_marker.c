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

/*
 * Provide the vcs advanced conflict marker classify operation used by this module and its
 * client applications.
 */
UmiVcsAdvancedConflictMarkerKind umi_vcs_advanced_conflict_marker_classify(
    const char *line)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (line == NULL) {
        return UMI_VCS_MARKER_NONE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(line, "<<<<<<<", 7U) == 0) {
        return UMI_VCS_MARKER_OURS;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(line, "|||||||", 7U) == 0) {
        return UMI_VCS_MARKER_BASE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(line, "=======", 7U) == 0) {
        return UMI_VCS_MARKER_SEPARATOR;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(line, ">>>>>>>", 7U) == 0) {
        return UMI_VCS_MARKER_THEIRS;
    }
    return UMI_VCS_MARKER_NONE;
}

/*
 * Provide the vcs advanced conflict marker balanced operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_conflict_marker_balanced(
    const char *text,
    size_t *out_conflicts)
{
    const char *cursor;
    const char *end;
    size_t conflicts = 0U;
    int state = 0;
    char line[256];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) {
        return 0;
    }

    cursor = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        size_t length;
        UmiVcsAdvancedConflictMarkerKind kind;

        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        kind = umi_vcs_advanced_conflict_marker_classify(line);

        /* Apply this branch only when its contract condition is satisfied. */
        if (kind == UMI_VCS_MARKER_OURS) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (state != 0) {
                return 0;
            }
            state = 1;
            conflicts += 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_VCS_MARKER_SEPARATOR) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (state != 1 && state != 2) {
                return 0;
            }
            state = 3;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_VCS_MARKER_BASE) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (state != 1) {
                return 0;
            }
            state = 2;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_VCS_MARKER_THEIRS) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (state != 3) {
                return 0;
            }
            state = 0;
        }

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_conflicts != NULL) {
        *out_conflicts = conflicts;
    }
    return state == 0;
}
