/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/git_output.c
 *
 * PURPOSE:
 *   Centralise safe Git output trimming, line access and numeric parsing.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/git_output.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* Trim captured output in-place while preserving an always-terminated string. */
UmiStatus umi_repository_git_output_trim(char *text)
{
    char *start;
    char *end;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    start = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*start != '\0' && isspace((unsigned char)*start)) ++start;
    /* Apply this branch only when its contract condition is satisfied. */
    if (start != text) (void)memmove(text, start, strlen(start) + 1U);
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_OK;
    end = text + length;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > text && isspace((unsigned char)end[-1])) --end;
    *end = '\0';
    return UMI_STATUS_OK;
}

/* Parse a bounded size value and reject malformed or negative text. */
UmiStatus umi_repository_git_output_parse_size(const char *text, size_t *out_value)
{
    char *end = NULL;
    unsigned long long value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_value == NULL || text[0] == '\0' || text[0] == '-') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    errno = 0;
    value = strtoull(text, &end, 10);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (errno != 0 || end == text || *end != '\0') return UMI_STATUS_PARSE_ERROR;
    *out_value = (size_t)value;
    return UMI_STATUS_OK;
}

/* Count non-empty Git output records without allocating temporary arrays. */
size_t umi_repository_git_output_line_count(const char *text)
{
    size_t count = 0U;
    int in_line = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*text == '\n' || *text == '\r') {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (in_line) ++count;
            in_line = 0;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            in_line = 1;
        }
        ++text;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (in_line) ++count;
    return count;
}

/* Extract one logical output record while normalising CRLF/LF separators. */
UmiStatus umi_repository_git_output_line_at(
    const char *text, size_t index, char *out_line, size_t capacity)
{
    size_t current = 0U;
    const char *start;
    const char *end;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_line == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    start = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*start != '\0') {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*start == '\n' || *start == '\r') ++start;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*start == '\0') break;
        end = start;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*end != '\0' && *end != '\n' && *end != '\r') ++end;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (current == index) {
            length = (size_t)(end - start);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            (void)memcpy(out_line, start, length);
            out_line[length] = '\0';
            return UMI_STATUS_OK;
        }
        ++current;
        start = end;
    }
    return UMI_STATUS_NOT_FOUND;
}
