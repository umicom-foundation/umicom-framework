/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/commit.c
 *
 * PURPOSE:
 *   Implement parsing of commit ID, timestamp, author, email and subject fields separated by unit separators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/commit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the copy field operation used by this module and its client applications. */
static UmiStatus copy_field(char *destination,
                            size_t capacity,
                            const char *start,
                            const char *end)
{
    size_t length = (size_t)(end - start);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, start, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs commit parse record operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_commit_parse_record(const char *record,
                                      UmiVcsCommit *out_commit)
{
    const char separator = '\x1f';
    const char *fields[5];
    const char *ends[5];
    const char *cursor;
    size_t index;
    char timestamp[64];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || out_commit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_commit, 0, sizeof(*out_commit));
    cursor = record;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 5U; ++index) {
        const char *end = index < 4U
            ? strchr(cursor, separator)
            : cursor + strcspn(cursor, "\r\n");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) {
            return UMI_STATUS_PARSE_ERROR;
        }
        fields[index] = cursor;
        ends[index] = end;
        cursor = end + (index < 4U ? 1 : 0);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (copy_field(out_commit->commit_id,
                   sizeof(out_commit->commit_id),
                   fields[0],
                   ends[0]) != UMI_STATUS_OK ||
        copy_field(timestamp,
                   sizeof(timestamp),
                   fields[1],
                   ends[1]) != UMI_STATUS_OK ||
        copy_field(out_commit->author,
                   sizeof(out_commit->author),
                   fields[2],
                   ends[2]) != UMI_STATUS_OK ||
        copy_field(out_commit->email,
                   sizeof(out_commit->email),
                   fields[3],
                   ends[3]) != UMI_STATUS_OK ||
        copy_field(out_commit->subject,
                   sizeof(out_commit->subject),
                   fields[4],
                   ends[4]) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_commit->timestamp = (int64_t)strtoll(timestamp, NULL, 10);
    return UMI_STATUS_OK;
}
