/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_policy.c
 *
 * PURPOSE:
 *   Keep private working records out of automatic broad staging and identify duplicate remotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Match ASCII policy tokens without changing UTF-8 source path bytes. */
static int same(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a++) != tolower((unsigned char)*b++))
            return 0;
    }
    return *a == *b;
}

static int prefix(const char *value, const char *start)
{
    while (*start) {
        if (!*value || tolower((unsigned char)*value++) !=
            tolower((unsigned char)*start++)) return 0;
    }
    return 1;
}

/* Batch/update source symbols are legitimate code; private delivery records
 * use document extensions. Avoid blocking src/batch_queue.c by filename alone. */
static int working_record(const char *name)
{
    const char *extension = strrchr(name, '.');
    if (!extension || (!prefix(name, "UPDATE_") && !prefix(name, "BATCH_")))
        return 0;
    return same(extension, ".md") || same(extension, ".txt") ||
        same(extension, ".html") || same(extension, ".htm") ||
        same(extension, ".json") || same(extension, ".csv") ||
        same(extension, ".log") || same(extension, ".docx") ||
        same(extension, ".xlsx") || same(extension, ".pdf");
}

int umi_repository_publication_private_path(const char *path)
{
    char part[UMI_PATH_CAPACITY];
    const char *cursor = path;
    size_t component = 0U;
    if (path == NULL || *path == '\0' || umi_path_is_absolute(path))
        return 1;
    /* Files under private tool directories remain private even if someone
     * accidentally tracked them before an ignore rule was introduced. */
    while (*cursor) {
        size_t length = strcspn(cursor, "/\\");
        if (!length || length >= sizeof(part)) return 1;
        memcpy(part, cursor, length);
        part[length] = '\0';
        for (size_t i = 0U; i < length; ++i)
            if ((unsigned char)part[i] < 32U || part[i] == 127) return 1;
        if (same(part, "..") || same(part, ".git") ||
            same(part, "_local") || same(part, ".codex") ||
            same(part, ".agents") || same(part, "private") ||
            working_record(part) ||
            same(part, "AGENTS.md") || same(part, "ROADMAP_MAJOR_UPDATES.md") ||
            same(part, "UMICOM_EDUCATION_NATIVE_BATCH.md") ||
            same(part, "SITE_MIGRATION.md") ||
            same(part, "CMakeCache.txt") || same(part, "build.ninja"))
            return 1;
        if ((same(part, ".env") || prefix(part, ".env.")) &&
            !same(part, ".env.example") && !same(part, ".env.sample"))
            return 1;
        if (component == 0U && (same(part, "build") || same(part, "out") ||
            same(part, "Debug") || same(part, "Release") ||
            prefix(part, "cmake-build-"))) return 1;
        cursor += length;
        if (*cursor) ++cursor;
        ++component;
    }
    return same(path, "docs/adr/0001-framework-first-healthcare.md");
}

UmiStatus umi_publication_origin_identity(
    const char *url, char *out, size_t capacity)
{
    const char *host;
    const char *separator;
    size_t length;
    if (url == NULL || out == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    /* Group HTTPS and Git's common SSH spelling of the same remote. Embedded
     * credentials, URL options and custom transports are refused, not echoed. */
    if (strncmp(url, "https://", 8U) == 0) {
        host = url + 8U;
        separator = strchr(host, '/');
    } else if (strncmp(url, "git@", 4U) == 0) {
        host = url + 4U;
        separator = strchr(host, ':');
    } else return UMI_STATUS_INVALID_ARGUMENT;
    if (!separator || separator == host || !separator[1])
        return UMI_STATUS_INVALID_ARGUMENT;
    for (const char *p = host; *p; ++p)
        if ((unsigned char)*p <= 32U || *p == '@' || *p == '?' ||
            *p == '#' || *p == '\\' || *p == '%')
            return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(host);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, host, length + 1U);
    out[separator - host] = '/';
    /* Hostnames are case-insensitive; repository paths remain case-sensitive
     * to avoid grouping different repositories on other Git hosting services. */
    for (size_t i = 0U; i < (size_t)(separator - host); ++i)
        out[i] = (char)tolower((unsigned char)out[i]);
    if (length > 4U && strcmp(out + length - 4U, ".git") == 0)
        out[length - 4U] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_publication_safe_path(UmiPublicationState *state, const char *path)
{
    char current[UMI_PATH_CAPACITY];
    UmiFileInfo *info = calloc(1U, sizeof(*info));
    UmiStatus status;
    if (!info) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_path_normalise(path, current, sizeof(current));
    if (status == UMI_STATUS_OK && !umi_path_is_within(state->root, current))
        status = UMI_STATUS_PERMISSION_DENIED;
    /* Reject junctions/symlinks along the owned path. This protects ordinary
     * mistakes; callers must still exclude concurrent filesystem replacement. */
    while (status == UMI_STATUS_OK) {
        status = umi_directory_stat(current, info);
        if (status != UMI_STATUS_OK) break;
        if (info->kind == UMI_FILE_KIND_SYMBOLIC_LINK) {
            status = UMI_STATUS_PERMISSION_DENIED;
            break;
        }
        if (umi_path_equal(current, state->root)) break;
        char parent[UMI_PATH_CAPACITY];
        status = umi_path_parent(current, parent, sizeof(parent));
        if (status == UMI_STATUS_OK) memcpy(current, parent, strlen(parent) + 1U);
    }
    free(info);
    return status;
}
