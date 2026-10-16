/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_discovery.c
 *
 * PURPOSE:
 *   Discover workspace repositories and registered Git children with bounded traversal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int vendor_path(const char *relative)
{
    return (strncmp(relative, "third_party/", 12U) == 0 &&
            strncmp(relative, "third_party/umicom/", 19U) != 0) ||
        strncmp(relative, "vendor/", 7U) == 0 ||
        strncmp(relative, "third-party/", 12U) == 0;
}

static UmiStatus discover_repo(UmiPublicationState *state, const char *path,
    size_t parent, int vendor, size_t depth, size_t *out_index)
{
    UmiRepositoryPublicationReport *report = state->report;
    UmiPublicationCapture capture;
    const char *arguments[] = {"ls-files", "--stage", "-z"};
    char git_path[UMI_PATH_CAPACITY];
    size_t index, offset = 0U;
    UmiStatus status;
    if (depth > 32U) return umi_publication_fail(state, parent,
        UMI_STATUS_CAPACITY_EXCEEDED, "Repository nesting exceeds 32 levels.");
    for (index = 0U; index < report->count; ++index) {
        if (umi_path_equal(path, report->entries[index].path)) {
            *out_index = index;
            return UMI_STATUS_OK;
        }
    }
    if (report->count == UMI_REPOSITORY_PUBLICATION_CAPACITY)
        return umi_publication_fail(state, parent, UMI_STATUS_CAPACITY_EXCEEDED,
            "Workspace contains more than 128 checkouts.");
    status = umi_publication_safe_path(state, path);
    if (status != UMI_STATUS_OK) return umi_publication_fail(state, parent,
        status, "A repository path escapes the workspace or traverses a link.");
    status = umi_path_join(path, ".git", git_path, sizeof(git_path));
    if (status != UMI_STATUS_OK || !umi_fs_exists(git_path))
        return umi_publication_fail(state, parent, UMI_STATUS_NOT_FOUND,
            "A registered child is missing its Git checkout; initialise it separately.");
    status = umi_publication_safe_path(state, git_path);
    if (status != UMI_STATUS_OK) return umi_publication_fail(state, parent,
        status, "Repository metadata is a link or lies outside the workspace.");
    index = report->count++;
    *out_index = index;
    UmiRepositoryPublicationEntry *entry = &report->entries[index];
    (void)snprintf(entry->path, sizeof(entry->path), "%s", path);
    entry->parent = parent;
    entry->primary = index;
    entry->vendor = vendor;
    status = umi_publication_git(state, index, arguments, 3U, 0, &capture);
    if (status != UMI_STATUS_OK) return status;
    /* Git's index is authoritative for registered child revisions. Source
     * directories are not recursively searched, so build caches are excluded. */
    while (offset < capture.size) {
        char *record = capture.bytes + offset;
        char *end = memchr(record, '\0', capture.size - offset);
        char *tab;
        char mode[7], sha[65], extra;
        unsigned stage;
        if (!end || !(tab = memchr(record, '\t', (size_t)(end - record)))) {
            status = UMI_STATUS_PARSE_ERROR; break;
        }
        *tab = '\0';
        if (sscanf(record, "%6s %64s %u%c", mode, sha, &stage, &extra) != 3 ||
            stage != 0U) {
            status = UMI_STATUS_INVALID_STATE; break;
        }
        if (strcmp(mode, "160000") == 0) {
            char child[UMI_PATH_CAPACITY];
            char relative[UMI_PATH_CAPACITY];
            size_t child_index;
            UmiPublicationEdge *edge;
            if (state->edge_count == UMI_PUBLICATION_EDGE_CAPACITY ||
                strlen(sha) < 40U || strlen(tab + 1) >= sizeof(relative) ||
                umi_path_is_absolute(tab + 1)) {
                status = UMI_STATUS_CAPACITY_EXCEEDED; break;
            }
            status = umi_path_normalise(tab + 1, relative, sizeof(relative));
            if (status != UMI_STATUS_OK || strcmp(relative, tab + 1) != 0 ||
                strstr(relative, "..") != NULL) {
                status = UMI_STATUS_INVALID_ARGUMENT; break;
            }
            status = umi_path_join(path, relative, child, sizeof(child));
            if (status != UMI_STATUS_OK) break;
            status = discover_repo(state, child, index,
                vendor || vendor_path(relative), depth + 1U, &child_index);
            if (status != UMI_STATUS_OK) break;
            /* Recursive discovery may consume the last edge slot. Recheck
             * after it returns before recording this parent relationship. */
            if (state->edge_count == UMI_PUBLICATION_EDGE_CAPACITY) {
                status = UMI_STATUS_CAPACITY_EXCEEDED; break;
            }
            edge = &state->edges[state->edge_count++];
            edge->parent = index;
            edge->child = child_index;
            (void)snprintf(edge->path, sizeof(edge->path), "%s", relative);
            (void)snprintf(edge->pinned, sizeof(edge->pinned), "%s", sha);
        }
        offset = (size_t)(end - capture.bytes) + 1U;
    }
    free(capture.bytes);
    if (status != UMI_STATUS_OK) return umi_publication_fail(state, index, status,
        "Cannot inspect every registered child or the index contains conflicts.");
    return UMI_STATUS_OK;
}

static UmiStatus visit_top(const UmiFileInfo *info, void *context)
{
    UmiPublicationState *state = context;
    char git_path[UMI_PATH_CAPACITY];
    size_t index;
    if (info->kind != UMI_FILE_KIND_DIRECTORY || info->name[0] == '.' ||
        strcmp(info->name, "_local") == 0 ||
        strcmp(info->name, "build") == 0 || strcmp(info->name, "installed") == 0)
        return UMI_STATUS_OK;
    UmiStatus status = umi_path_join(info->path, ".git", git_path, sizeof(git_path));
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_exists(git_path)) return UMI_STATUS_OK;
    return discover_repo(state, info->path, SIZE_MAX, 0, 0U, &index);
}

UmiStatus umi_publication_discover(UmiPublicationState *state)
{
    char git_path[UMI_PATH_CAPACITY];
    size_t index;
    UmiStatus status = umi_publication_safe_path(state, state->root);
    UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
    if (status != UMI_STATUS_OK) return status;
    status = umi_path_join(state->root, ".git", git_path, sizeof(git_path));
    if (status != UMI_STATUS_OK) return status;
    if (umi_fs_exists(git_path))
        return discover_repo(state, state->root, SIZE_MAX, 0, 0U, &index);
    options.recursive = 0;
    options.include_files = 0;
    options.include_directories = 1;
    options.include_hidden = 0;
    options.follow_symbolic_links = 0;
    status = UmiDirectoryWalkCancellable(state->root, &options, visit_top, state,
        state->request->cancellation);
    if (status == UMI_STATUS_OK && state->report->count == 0U)
        return umi_publication_fail(state, SIZE_MAX, UMI_STATUS_NOT_FOUND,
            "No immediate repositories were found in the selected workspace.");
    return status;
}
