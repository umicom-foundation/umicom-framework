/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/unified_diff.c
 *
 * PURPOSE:
 *   Parse common Git unified-diff output for professional source-control views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/unified_diff.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Read developer unified diff into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_developer_unified_diff_parse(
    const char *text,
    UmiDeveloperUnifiedDiff *out_diff)
{
    const char *cursor;
    size_t old_line = 0U;
    size_t new_line = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_diff == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_diff, 0, sizeof(*out_diff));
    cursor = text;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL
            ? (size_t)(end - cursor)
            : strlen(cursor);
        char line[UMI_DEVELOPER_PRODUCTIVITY_LINE_CAPACITY];
        UmiDeveloperUnifiedDiffLine *record;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line) ||
            out_diff->line_count >=
                UMI_DEVELOPER_UNIFIED_DIFF_LINE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(line, cursor, length);
        line[length] = '\0';

        record = &out_diff->lines[out_diff->line_count++];
        copy_text(record->text, sizeof(record->text), line);

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strncmp(line, "@@ ", 3U) == 0) {
            unsigned long old_start = 0UL;
            unsigned long new_start = 0UL;

            record->kind = UMI_DEVELOPER_UNIFIED_DIFF_HUNK;
            out_diff->hunks += 1U;

            /* Apply this branch only when its contract condition is satisfied. */
            if (sscanf(
                    line,
                    "@@ -%lu%*[^+] +%lu",
                    &old_start,
                    &new_start) == 2) {
                old_line = (size_t)old_start;
                new_line = (size_t)new_start;
            }
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(line, "---", 3U) == 0 ||
                   strncmp(line, "+++", 3U) == 0 ||
                   strncmp(line, "diff ", 5U) == 0 ||
                   strncmp(line, "index ", 6U) == 0) {
            record->kind = UMI_DEVELOPER_UNIFIED_DIFF_HEADER;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (line[0] == '+' && line[1] != '+') {
            record->kind = UMI_DEVELOPER_UNIFIED_DIFF_ADDITION;
            record->new_line = new_line++;
            out_diff->additions += 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (line[0] == '-' && line[1] != '-') {
            record->kind = UMI_DEVELOPER_UNIFIED_DIFF_DELETION;
            record->old_line = old_line++;
            out_diff->deletions += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            record->kind = UMI_DEVELOPER_UNIFIED_DIFF_CONTEXT;
            record->old_line = old_line++;
            record->new_line = new_line++;
        }

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }

    return UMI_STATUS_OK;
}
