/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/branch.c
 *
 * PURPOSE:
 *   Implement branch parser and concise display formatting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/branch.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_vcs_branch_parse_status_header(const char *line,
                                             UmiVcsBranch *out_branch)
{
    const char *name_start;
    const char *upstream_start;
    const char *bracket;
    const char *end;
    size_t length;
    if (line == NULL || out_branch == NULL ||
        strncmp(line, "## ", 3U) != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_branch, 0, sizeof(*out_branch));
    name_start = line + 3;
    if (strncmp(name_start, "HEAD (no branch)", 16U) == 0) {
        out_branch->detached = 1;
        (void)snprintf(out_branch->name,
                       sizeof(out_branch->name),
                       "%s",
                       "HEAD");
        return UMI_STATUS_OK;
    }
    upstream_start = strstr(name_start, "...");
    bracket = strchr(name_start, '[');
    end = upstream_start != NULL ? upstream_start :
          (bracket != NULL ? bracket : name_start + strlen(name_start));
    while (end > name_start && end[-1] == ' ') {
        end -= 1;
    }
    length = (size_t)(end - name_start);
    if (length >= sizeof(out_branch->name)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_branch->name, name_start, length);
    out_branch->name[length] = '\0';
    out_branch->current = 1;
    if (upstream_start != NULL) {
        const char *upstream = upstream_start + 3;
        const char *upstream_end = bracket != NULL
            ? bracket
            : name_start + strlen(name_start);
        while (upstream_end > upstream && upstream_end[-1] == ' ') {
            upstream_end -= 1;
        }
        length = (size_t)(upstream_end - upstream);
        if (length >= sizeof(out_branch->upstream)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(out_branch->upstream, upstream, length);
        out_branch->upstream[length] = '\0';
    }
    if (bracket != NULL) {
        const char *ahead = strstr(bracket, "ahead ");
        const char *behind = strstr(bracket, "behind ");
        if (ahead != NULL) {
            (void)sscanf(ahead, "ahead %d", &out_branch->ahead);
        }
        if (behind != NULL) {
            (void)sscanf(behind, "behind %d", &out_branch->behind);
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_branch_parse_current(const char *output,
                                       UmiVcsBranch *out_branch)
{
    const char *newline;
    size_t length;
    if (output == NULL || out_branch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_branch, 0, sizeof(*out_branch));
    newline = strpbrk(output, "\r\n");
    length = newline != NULL ? (size_t)(newline - output) : strlen(output);
    if (length == 0U || length >= sizeof(out_branch->name)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    (void)memcpy(out_branch->name, output, length);
    out_branch->name[length] = '\0';
    out_branch->current = 1;
    out_branch->detached = strcmp(out_branch->name, "HEAD") == 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_branch_format(const UmiVcsBranch *branch,
                                char *out_text,
                                size_t capacity)
{
    int written;
    if (branch == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text,
                       capacity,
                       "%s%s%s%s ahead=%d behind=%d",
                       branch->name,
                       branch->detached ? " (detached)" : "",
                       branch->upstream[0] != '\0' ? " -> " : "",
                       branch->upstream,
                       branch->ahead,
                       branch->behind);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}
