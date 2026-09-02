/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/diff.c
 *
 * PURPOSE:
 *   Implement numstat parsing for textual and binary file changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/diff.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Provide the vcs diff parse numstat operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_diff_parse_numstat(const char *output,
                                     UmiVcsDiffStat *out_stats,
                                     size_t capacity,
                                     size_t *out_count)
{
    const char *cursor;
    const char *end;
    char line[UMI_VCS_PATH_CAPACITY + 128U];
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || out_stats == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cursor = output;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        char insertions[32];
        char deletions[32];
        char path[UMI_VCS_PATH_CAPACITY];
        size_t length;
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        /* Apply this branch only when its contract condition is satisfied. */
        if (sscanf(line,
                   "%31[^\t]\t%31[^\t]\t%2047[^\r\n]",
                   insertions,
                   deletions,
                   path) == 3) {
            UmiVcsDiffStat *stat;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (count >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            stat = &out_stats[count++];
            (void)memset(stat, 0, sizeof(*stat));
            (void)snprintf(stat->path, sizeof(stat->path), "%s", path);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(insertions, "-") == 0 ||
                strcmp(deletions, "-") == 0) {
                stat->binary = 1;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                stat->insertions = (size_t)strtoull(insertions, NULL, 10);
                stat->deletions = (size_t)strtoull(deletions, NULL, 10);
            }
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
    if (out_count != NULL) {
        *out_count = count;
    }
    return UMI_STATUS_OK;
}

/* Provide the vcs diff totals operation used by this module and its client applications. */
void umi_vcs_diff_totals(const UmiVcsDiffStat *stats,
                         size_t count,
                         size_t *out_insertions,
                         size_t *out_deletions,
                         size_t *out_binary_files)
{
    size_t index;
    size_t insertions = 0U;
    size_t deletions = 0U;
    size_t binary = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; stats != NULL && index < count; ++index) {
        insertions += stats[index].insertions;
        deletions += stats[index].deletions;
        binary += stats[index].binary ? 1U : 0U;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_insertions != NULL) *out_insertions = insertions;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_deletions != NULL) *out_deletions = deletions;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_binary_files != NULL) *out_binary_files = binary;
}
