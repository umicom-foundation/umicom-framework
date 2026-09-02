/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/patch_parser.c
 *
 * PURPOSE:
 *   Implement unified-diff metadata parsing without applying repository mutations.
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

#include "umicom/vcs/advanced/patch_parser.h"

#include <string.h>

/*
 * Provide the vcs advanced patch parse summary operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_patch_parse_summary(
    const char *patch,
    UmiVcsAdvancedPatchSummary *out_summary)
{
    const char *cursor;
    const char *end;
    int seen_old_header = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || out_summary == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_summary, 0, sizeof(*out_summary));
    out_summary->struct_size = (uint32_t)sizeof(*out_summary);
    out_summary->api_version = UMI_VCS_ADVANCED_API_VERSION;
    cursor = patch;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        char line[1024];
        size_t length;

        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strncmp(line, "diff --git ", 11U) == 0) {
            out_summary->file_count += 1U;
            seen_old_header = 0;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(line, "--- ", 4U) == 0) {
            seen_old_header = 1;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(line, "+++ ", 4U) == 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (!seen_old_header) {
                out_summary->malformed = 1;
            }
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(line, "@@", 2U) == 0) {
            out_summary->hunk_count += 1U;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (line[0] == '+' && strncmp(line, "+++", 3U) != 0) {
            out_summary->added_lines += 1U;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (line[0] == '-' && strncmp(line, "---", 3U) != 0) {
            out_summary->deleted_lines += 1U;
        } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strstr(line, "Binary files ") != NULL) {
            out_summary->has_binary = 1;
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

    return out_summary->malformed ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
}
