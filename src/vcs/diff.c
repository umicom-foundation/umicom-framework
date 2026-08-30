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

UmiStatus umi_vcs_diff_parse_numstat(const char *output,
                                     UmiVcsDiffStat *out_stats,
                                     size_t capacity,
                                     size_t *out_count)
{
    const char *cursor;
    const char *end;
    char line[UMI_VCS_PATH_CAPACITY + 128U];
    size_t count = 0U;
    if (output == NULL || out_stats == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cursor = output;
    while (*cursor != '\0') {
        char insertions[32];
        char deletions[32];
        char path[UMI_VCS_PATH_CAPACITY];
        size_t length;
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        if (sscanf(line,
                   "%31[^\t]\t%31[^\t]\t%2047[^\r\n]",
                   insertions,
                   deletions,
                   path) == 3) {
            UmiVcsDiffStat *stat;
            if (count >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            stat = &out_stats[count++];
            (void)memset(stat, 0, sizeof(*stat));
            (void)snprintf(stat->path, sizeof(stat->path), "%s", path);
            if (strcmp(insertions, "-") == 0 ||
                strcmp(deletions, "-") == 0) {
                stat->binary = 1;
            } else {
                stat->insertions = (size_t)strtoull(insertions, NULL, 10);
                stat->deletions = (size_t)strtoull(deletions, NULL, 10);
            }
        }
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    if (out_count != NULL) {
        *out_count = count;
    }
    return UMI_STATUS_OK;
}

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
    for (index = 0U; stats != NULL && index < count; ++index) {
        insertions += stats[index].insertions;
        deletions += stats[index].deletions;
        binary += stats[index].binary ? 1U : 0U;
    }
    if (out_insertions != NULL) *out_insertions = insertions;
    if (out_deletions != NULL) *out_deletions = deletions;
    if (out_binary_files != NULL) *out_binary_files = binary;
}
