/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_inspection.c
 *
 * PURPOSE:
 *   Preflight branches, origin targets, private paths and unpublished histories before mutation.
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

UmiStatus umi_publication_check_paths(UmiPublicationState *state, size_t index,
    const char *const *args, size_t count)
{
    UmiPublicationCapture capture;
    UmiStatus status = umi_publication_git(state, index, args, count, 0, &capture);
    size_t offset = 0U;
    if (status != UMI_STATUS_OK) return status;
    while (offset < capture.size) {
        char *path = capture.bytes + offset;
        char *end = memchr(path, '\0', capture.size - offset);
        if (!end || end == path) { status = UMI_STATUS_PARSE_ERROR; break; }
        if (umi_repository_publication_private_path(path)) {
            char detail[512];
            (void)snprintf(detail, sizeof(detail), "Review private/generated path before publishing: %.400s", path);
            free(capture.bytes);
            return umi_publication_fail(state, index, UMI_STATUS_PERMISSION_DENIED, detail);
        }
        offset = (size_t)(end - capture.bytes) + 1U;
    }
    free(capture.bytes);
    return status;
}

UmiStatus umi_publication_check_history(UmiPublicationState *state, size_t index)
{
    UmiRepositoryPublicationEntry *entry = &state->report->entries[index];
    UmiPublicationCapture capture;
    char range[140];
    const char *args[] = {"rev-list", range};
    UmiStatus status;
    if (strcmp(entry->head, entry->remote_head) == 0) return UMI_STATUS_OK;
    if (entry->ahead > UMI_PUBLICATION_HISTORY_LIMIT)
        return umi_publication_fail(state, index, UMI_STATUS_CAPACITY_EXCEEDED,
            "More than 256 unpublished commits require a separate publication review.");
    (void)snprintf(range, sizeof(range), "%s..HEAD", entry->remote_head);
    status = umi_publication_git(state, index, args, 2U, 0, &capture);
    if (status != UMI_STATUS_OK) return status;
    /* Inspect each unpublished tree, including an intermediate commit that
     * added a private file and a later commit that removed it. */
    char *cursor = capture.bytes;
    size_t seen = 0U;
    while (*cursor) {
        char *end = strchr(cursor, '\n');
        if (end) *end = '\0';
        size_t length = strlen(cursor);
        if (length && cursor[length - 1U] == '\r') cursor[--length] = '\0';
        if ((length != 40U && length != 64U) ||
            strspn(cursor, "0123456789abcdefABCDEF") != length ||
            ++seen > UMI_PUBLICATION_HISTORY_LIMIT) {
            status = UMI_STATUS_PARSE_ERROR; break;
        }
        const char *tree[] = {"ls-tree", "-r", "--name-only", "-z", cursor};
        status = umi_publication_check_paths(state, index, tree, 5U);
        if (status != UMI_STATUS_OK || !end) break;
        cursor = end + 1;
    }
    free(capture.bytes);
    return status;
}

/* A status record is two status bytes, a space, and one NUL-delimited path.
 * Renames/copies carry a second path; consume it without mistaking it for
 * another record. Gitlinks are handled by the dependency graph separately. */
static UmiStatus inspect_status(UmiPublicationState *state, size_t index)
{
    const char *args[] = {"status", "--porcelain=v1", "-z",
        "--untracked-files=all", "--ignore-submodules=all"};
    UmiPublicationCapture capture;
    UmiStatus status = umi_publication_git(state, index, args, 5U, 0, &capture);
    size_t offset = 0U;
    if (status != UMI_STATUS_OK) return status;
    state->report->entries[index].changed_paths = 0U;
    state->dirty[index] = capture.size != 0U;
    while (offset < capture.size) {
        char *record = capture.bytes + offset;
        char *end = memchr(record, '\0', capture.size - offset);
        int gitlink = 0;
        if (!end || end - record < 4 || record[2] != ' ' ||
            record[0] == 'U' || record[1] == 'U') {
            status = UMI_STATUS_PARSE_ERROR; break;
        }
        for (size_t e = 0U; e < state->edge_count; ++e)
            if (state->edges[e].parent == index &&
                strcmp(state->edges[e].path, record + 3) == 0) gitlink = 1;
        if (!gitlink) ++state->report->entries[index].changed_paths;
        if (strcmp(record + 3, ".gitmodules") == 0) {
            free(capture.bytes);
            return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
                "Commit and review submodule registration changes before ecosystem publication.");
        }
        offset = (size_t)(end - capture.bytes) + 1U;
        if (record[0] == 'R' || record[1] == 'R' ||
            record[0] == 'C' || record[1] == 'C') {
            end = memchr(capture.bytes + offset, '\0', capture.size - offset);
            if (!end) { status = UMI_STATUS_PARSE_ERROR; break; }
            offset = (size_t)(end - capture.bytes) + 1U;
        }
    }
    free(capture.bytes);
    return status;
}

UmiStatus umi_publication_inspect(UmiPublicationState *state, size_t index)
{
    UmiRepositoryPublicationEntry *entry = &state->report->entries[index];
    char text[UMI_PATH_CAPACITY], push_identity[1024], git_dir[UMI_PATH_CAPACITY];
    const char *root[] = {"rev-parse", "--show-toplevel"};
    const char *directory[] = {"rev-parse", "--absolute-git-dir"};
    const char *head[] = {"rev-parse", "--verify", "HEAD"};
    const char *branch[] = {"symbolic-ref", "--quiet", "--short", "HEAD"};
    const char *origin[] = {"remote", "get-url", "--all", "origin"};
    const char *push[] = {"remote", "get-url", "--push", "--all", "origin"};
    const char *fetch[] = {"fetch", "--no-tags", "--no-recurse-submodules", "origin",
        "refs/heads/main:refs/remotes/origin/main"};
    const char *remote[] = {"rev-parse", "--verify", "refs/remotes/origin/main"};
    const char *files[] = {"ls-files", "--cached", "--others", "--exclude-standard", "-z"};
    const char *markers[] = {"index.lock", "MERGE_HEAD", "CHERRY_PICK_HEAD",
        "REVERT_HEAD", "rebase-merge", "rebase-apply", "BISECT_LOG"};
    UmiStatus status = umi_publication_text(state, index, root, 2U, text, sizeof(text));
    if (status != UMI_STATUS_OK) return status;
    if (!umi_path_equal(text, entry->path))
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Git resolved a different checkout root.");
    status = umi_publication_text(state, index, directory, 2U, git_dir, sizeof(git_dir));
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_safe_path(state, git_dir);
    if (status != UMI_STATUS_OK) return umi_publication_fail(state, index, status,
        "Git metadata is outside the selected workspace or traverses a link.");
    for (size_t m = 0U; m < sizeof(markers) / sizeof(markers[0]); ++m) {
        status = umi_path_join(git_dir, markers[m], text, sizeof(text));
        if (status != UMI_STATUS_OK) return status;
        if (umi_fs_exists(text)) return umi_publication_fail(state, index,
            UMI_STATUS_BUSY, "A Git lock or unfinished history operation needs review.");
    }
    status = umi_publication_text(state, index, head, 3U, entry->head, sizeof(entry->head));
    if (status != UMI_STATUS_OK) return status;
    status = inspect_status(state, index);
    if (status != UMI_STATUS_OK) return status;
    if (entry->vendor) {
        /* Vendor branches may be detached or named master. Their owning parent
         * pin is authoritative; this automation never selects a newer version. */
        for (size_t e = 0U; e < state->edge_count; ++e)
            if (state->edges[e].child == index &&
                (strcmp(state->edges[e].pinned, entry->head) != 0 || entry->changed_paths))
                return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
                    "Vendor checkout differs from its approved parent pin or has local changes.");
        return UMI_STATUS_OK;
    }
/* Distinguish detached HEAD from a Git transport failure before considering explicit recovery. The former implementation is retained for review. */
#if 0
    status = umi_publication_text(state, index, branch, 4U, entry->branch, sizeof(entry->branch));
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(entry->branch, "main") != 0)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Owned checkouts must already be on main; no branch switch was attempted.");
#endif
    (void)branch; /* The compatibility command remains documented above. */
    status = umi_publication_branch(state, index);
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_text(state, index, origin, 4U, text, sizeof(text));
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_origin_identity(text, state->identity[index], sizeof(state->identity[index]));
    if (status != UMI_STATUS_OK) return umi_publication_fail(state, index, status,
        "Origin must be one credential-free HTTPS or git@host:path remote.");
    status = umi_publication_text(state, index, push, 5U, text, sizeof(text));
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_origin_identity(text, push_identity, sizeof(push_identity));
    if (status != UMI_STATUS_OK || strcmp(push_identity, state->identity[index]) != 0)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Origin fetch and push destinations differ or contain multiple URLs.");
    if (state->request->refresh) {
        UmiPublicationCapture capture;
        status = umi_publication_git(state, index, fetch, 5U, 0, &capture);
        free(capture.bytes);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_publication_text(state, index, remote, 3U, entry->remote_head, sizeof(entry->remote_head));
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(text, sizeof(text), "HEAD...%s", entry->remote_head);
    const char *counts[] = {"rev-list", "--left-right", "--count", text};
    char numbers[128], extra;
    status = umi_publication_text(state, index, counts, 4U, numbers, sizeof(numbers));
    if (status != UMI_STATUS_OK) return status;
    if (sscanf(numbers, "%zu %zu%c", &entry->ahead, &entry->behind, &extra) != 2)
        return umi_publication_fail(state, index, UMI_STATUS_PARSE_ERROR,
            "Git returned invalid ahead/behind counts.");
    if ((entry->ahead && entry->behind) || (entry->behind && state->dirty[index]))
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Divergence or local edits behind origin/main require a reviewed reconciliation.");
    status = umi_publication_attachment_check(state, index);
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_check_paths(state, index, files, 5U);
    if (status == UMI_STATUS_OK) status = umi_publication_check_history(state, index);
    return status;
}
