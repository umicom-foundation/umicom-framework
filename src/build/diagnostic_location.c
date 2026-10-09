/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/diagnostic_location.c
 * PURPOSE: Resolve compiler source locations without guessing an implicit working directory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/diagnostic_location.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <string.h>

/* A compiler message is data. Refuse control characters and URI handlers at
 * this boundary; source navigation reads an ordinary local file only. */
static int DiagnosticLocalPath(const char *path, size_t capacity, int allow_empty)
{
    if (path == NULL || memchr(path, '\0', capacity) == NULL)
        return 0;
    if (path[0] == '\0')
        return allow_empty;
    if (path[0] == '<' || strstr(path, "://") != NULL)
        return 0;
    for (size_t index = 0U; path[index] != '\0'; ++index)
        if ((unsigned char)path[index] < 32U || (unsigned char)path[index] == 127U)
            return 0;
#ifdef _WIN32
    if (path[0] == '\\' || path[0] == '/')
        return 0;
    /* Drive-relative paths depend on hidden per-drive state, and extra colons
     * select NTFS streams. Source navigation accepts neither interpretation. */
    const char *colon = strchr(path, ':');
    if (colon != NULL &&
        (colon != path + 1U || !umi_path_is_absolute(path) || strchr(colon + 1U, ':') != NULL))
        return 0;
#endif
    return 1;
}
static int DiagnosticWithinRoots(const UmiBuildDiagnosticPage *page, const char *path)
{
    return (umi_path_is_absolute(page->source_directory) &&
            umi_path_is_within(page->source_directory, path)) ||
           (umi_path_is_absolute(page->build_directory) &&
            umi_path_is_within(page->build_directory, path));
}
UmiStatus UmiBuildDiagnosticResolveLocation(const UmiBuildDiagnosticPage *page, size_t row,
                                            UmiBuildDiagnosticLocation *out_location)
{
    if (page == NULL || out_location == NULL || page->count > UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY ||
        page->retained_count > UMI_BUILD_MAX_DIAGNOSTICS ||
        page->first_index > page->retained_count ||
        page->count > page->retained_count - page->first_index || row >= page->count ||
        page->progress.operation_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiBuildDiagnostic *item = &page->items[row];
    if (!DiagnosticLocalPath(item->file, sizeof item->file, 0) ||
        !DiagnosticLocalPath(page->source_directory, sizeof page->source_directory, 1) ||
        !DiagnosticLocalPath(page->build_directory, sizeof page->build_directory, 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (item->line == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (item->line > UINT32_MAX || item->column > UINT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiBuildDiagnosticLocation chosen = {0};
    if (umi_path_is_absolute(item->file))
    {
        UmiStatus status = umi_path_normalise(item->file, chosen.path, sizeof chosen.path);
        if (status != UMI_STATUS_OK)
            return status;
        if (!umi_fs_is_file(chosen.path))
            return UMI_STATUS_NOT_FOUND;
    }
    else
    {
        const char *roots[] = {page->source_directory, page->build_directory};
        for (size_t index = 0U; index < 2U; ++index)
        {
            if (!umi_path_is_absolute(roots[index]))
                continue;
            char candidate[UMI_BUILD_PATH_CAPACITY];
            UmiStatus status =
                umi_path_absolute(item->file, roots[index], candidate, sizeof candidate);
            if (status != UMI_STATUS_OK)
                return status;
            if (!DiagnosticWithinRoots(page, candidate) || !umi_fs_is_file(candidate))
                continue;
            if (chosen.path[0] != '\0' && !umi_path_equal(chosen.path, candidate))
                return UMI_STATUS_INVALID_STATE;
            memcpy(chosen.path, candidate, strlen(candidate) + 1U);
        }
        if (chosen.path[0] == '\0')
            return UMI_STATUS_NOT_FOUND;
    }
    chosen.line = (uint32_t)item->line;
    chosen.column = item->column == 0U ? 1U : (uint32_t)item->column;
    *out_location = chosen;
    return UMI_STATUS_OK;
}
