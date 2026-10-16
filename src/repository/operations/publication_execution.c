/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_execution.c
 *
 * PURPOSE:
 *   Publish children once, align duplicate copies and lock parents with explicit failure boundaries.
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

static void completed(UmiPublicationState *state, size_t index, const char *action)
{
    if (state->request->observer)
        state->request->observer(state->report->entries[index].path, action,
            state->request->observer_context);
}

static UmiStatus command(UmiPublicationState *state, size_t index,
    const char *const *args, size_t count)
{
    UmiPublicationCapture capture;
    UmiStatus status = umi_publication_git(state, index, args, count, 0, &capture);
    free(capture.bytes);
    return status;
}

/* Recheck the branch and commit immediately before a mutation. A successful
 * preview is not permission to switch away from work created in the meantime. */
static UmiStatus verify_head(UmiPublicationState *state, size_t index)
{
    const char *head[] = {"rev-parse", "--verify", "HEAD"};
    const char *branch[] = {"symbolic-ref", "--quiet", "--short", "HEAD"};
    char value[128];
    UmiStatus status = umi_publication_text(state, index, branch, 4U, value, sizeof(value));
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(value, "main") != 0) return umi_publication_fail(state, index,
        UMI_STATUS_INVALID_STATE, "Branch changed after inspection; publication stopped.");
    status = umi_publication_text(state, index, head, 3U, value, sizeof(value));
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(value, state->report->entries[index].head) != 0)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "HEAD changed after inspection; publication stopped.");
    return UMI_STATUS_OK;
}

static UmiStatus verify_origin(UmiPublicationState *state, size_t index)
{
    const char *fetch[] = {"remote", "get-url", "--all", "origin"};
    const char *push[] = {"remote", "get-url", "--push", "--all", "origin"};
    char url[UMI_PATH_CAPACITY], identity[1024];
    for (unsigned mode = 0U; mode < 2U; ++mode) {
        UmiStatus status = umi_publication_text(state, index, mode ? push : fetch,
            mode ? 5U : 4U, url, sizeof(url));
        if (status != UMI_STATUS_OK) return status;
        status = umi_publication_origin_identity(url, identity, sizeof(identity));
        if (status != UMI_STATUS_OK || strcmp(identity, state->identity[index]) != 0)
            return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
                "Origin changed after inspection; no further remote operation was attempted.");
    }
    return UMI_STATUS_OK;
}

static UmiStatus remote_is(UmiPublicationState *state, size_t index, const char *expected)
{
    const char *args[] = {"ls-remote", "--exit-code", "origin", "refs/heads/main"};
    char text[160], sha[65], ref[64], extra;
    UmiStatus status = verify_origin(state, index);
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_text(state, index, args, 4U, text, sizeof(text));
    if (status != UMI_STATUS_OK) return status;
    if (sscanf(text, "%64s %63s%c", sha, ref, &extra) != 2 ||
        strcmp(ref, "refs/heads/main") != 0 || strcmp(sha, expected) != 0)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Remote main changed during publication; inspect completed actions and retry.");
    return UMI_STATUS_OK;
}

/* After staging or updating a duplicate, reject newly introduced or missing
 * gitlinks instead of silently publishing a graph that was never planned. */
UmiStatus umi_publication_verify_links(UmiPublicationState *state, size_t index)
{
    const char *args[] = {"ls-files", "--stage", "-z"};
    UmiPublicationCapture capture;
    size_t offset = 0U, found = 0U, expected = 0U;
    UmiStatus status = umi_publication_git(state, index, args, 3U, 0, &capture);
    if (status != UMI_STATUS_OK) return status;
    for (size_t e = 0U; e < state->edge_count; ++e)
        if (state->edges[e].parent == index) ++expected;
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
        if (sscanf(record, "%6s %64s %u%c", mode, sha, &stage, &extra) != 3 || stage) {
            status = UMI_STATUS_INVALID_STATE; break;
        }
        if (strcmp(mode, "160000") == 0) {
            int match = 0;
            for (size_t e = 0U; e < state->edge_count; ++e) {
                UmiPublicationEdge *edge = &state->edges[e];
                if (edge->parent == index && strcmp(edge->path, tab + 1) == 0 &&
                    strcmp(sha, state->report->entries[edge->child].head) == 0)
                    match = 1;
            }
            if (!match) { status = UMI_STATUS_INVALID_STATE; break; }
            ++found;
        }
        offset = (size_t)(end - capture.bytes) + 1U;
    }
    free(capture.bytes);
    if (status != UMI_STATUS_OK || found != expected)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Submodule topology or staged revision changed; review the retained index.");
    return UMI_STATUS_OK;
}

static UmiStatus fast_forward(UmiPublicationState *state, size_t index, const char *target)
{
    const char *args[] = {"merge", "--ff-only", "--no-edit", target};
    const char *head[] = {"rev-parse", "--verify", "HEAD"};
    UmiStatus status = verify_head(state, index);
    if (status != UMI_STATUS_OK) return status;
    status = command(state, index, args, 4U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_text(state, index, head, 3U,
        state->report->entries[index].head, sizeof(state->report->entries[index].head));
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(state->report->entries[index].head, target) != 0)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Fast-forward did not produce the expected commit.");
    state->report->entries[index].fast_forwarded = 1;
    ++state->report->updated_count;
    completed(state, index, "Fast-forwarded");
    return UMI_STATUS_OK;
}

UmiStatus umi_publication_update_initial(UmiPublicationState *state, int *updated)
{
    *updated = 0;
    for (size_t i = 0U; i < state->report->count; ++i) {
        UmiRepositoryPublicationEntry *entry = &state->report->entries[i];
        if (!entry->vendor && entry->behind) {
            UmiStatus status = fast_forward(state, i, entry->remote_head);
            if (status != UMI_STATUS_OK) return status;
            *updated = 1;
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus publish_primary(UmiPublicationState *state, size_t index)
{
    UmiRepositoryPublicationEntry *entry = &state->report->entries[index];
    const char *add[] = {"add", "-A"};
    const char *check[] = {"diff", "--cached", "--check"};
    const char *quiet[] = {"diff", "--cached", "--quiet"};
    const char *files[] = {"ls-files", "--cached", "-z"};
    const char *tree[] = {"write-tree"};
    const char *committed_tree[] = {"rev-parse", "HEAD^{tree}"};
    const char *head[] = {"rev-parse", "--verify", "HEAD"};
    const char *commit[] = {"commit", "-m", state->request->commit_message ?
        state->request->commit_message : "chore: update source, tests and public documentation"};
    const char *push[] = {"push", "--recurse-submodules=check", "--no-follow-tags", "origin",
        "refs/heads/main:refs/heads/main"};
    UmiPublicationCapture capture;
    char planned_tree[65], actual_tree[65];
    int parent = 0;
    UmiStatus status = verify_head(state, index);
    if (status != UMI_STATUS_OK) return status;
    status = remote_is(state, index, entry->remote_head);
    if (status != UMI_STATUS_OK) return status;
    for (size_t e = 0U; e < state->edge_count; ++e)
        if (state->edges[e].parent == index) parent = 1;
    if (parent) {
        status = state->backend.lock(state->backend.context, entry->path, 0);
        if (status != UMI_STATUS_OK) return umi_publication_fail(state, index, status,
            "Parent lock failed; any staged work is preserved for review.");
        entry->locked = 1;
        completed(state, index, "Locked child revisions");
    }
    status = command(state, index, add, 2U);
    if (status == UMI_STATUS_OK) status = command(state, index, check, 3U);
    if (status == UMI_STATUS_OK) status = umi_publication_verify_links(state, index);
    if (status == UMI_STATUS_OK) status = umi_publication_check_paths(state, index, files, 3U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_publication_git(state, index, quiet, 3U, 1, &capture);
    if (status != UMI_STATUS_OK) return status;
    int changed = capture.exit_code == 1;
    free(capture.bytes);
    if (changed) {
        status = umi_publication_text(state, index, tree, 1U, planned_tree, sizeof(planned_tree));
        if (status == UMI_STATUS_OK) status = command(state, index, commit, 3U);
        if (status != UMI_STATUS_OK) return status;
        entry->committed = 1;
        ++state->report->committed_count;
        completed(state, index, "Committed locally");
        status = umi_publication_text(state, index, committed_tree, 2U,
            actual_tree, sizeof(actual_tree));
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(planned_tree, actual_tree) != 0)
            return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
                "A commit hook changed the reviewed tree; local commit retained without pushing.");
        status = umi_publication_text(state, index, head, 3U, entry->head, sizeof(entry->head));
        if (status != UMI_STATUS_OK) return status;
    }
    /* A previously committed but unpushed change is published even when the
     * worktree needs no new commit. Clean, current repositories skip pushing. */
    status = verify_head(state, index);
    if (status != UMI_STATUS_OK) return status;
    status = verify_origin(state, index);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(entry->head, entry->remote_head) != 0) {
        /* Reinspect unpublished trees after commit hooks, including any
         * intermediate commits they introduced before the final tree. */
        status = umi_publication_check_history(state, index);
        if (status != UMI_STATUS_OK) return status;
        status = command(state, index, push, 5U);
        if (status != UMI_STATUS_OK) return status;
        entry->pushed = 1;
        ++state->report->pushed_count;
        completed(state, index, "Pushed main");
    }
    return remote_is(state, index, entry->head);
}

UmiStatus umi_publication_execute(UmiPublicationState *state)
{
    for (size_t position = 0U; position < state->order_count; ++position) {
        size_t primary = state->order[position];
        UmiRepositoryPublicationEntry *source = &state->report->entries[primary];
        UmiStatus status = publish_primary(state, primary);
        if (status != UMI_STATUS_OK) return status;
        for (size_t i = 0U; i < state->report->count; ++i) {
            UmiRepositoryPublicationEntry *copy = &state->report->entries[i];
            if (i == primary || copy->vendor || copy->primary != primary) continue;
            if (strcmp(copy->head, source->head) != 0) {
                /* Reinspect before moving a duplicate: edits made after
                 * preview are never overwritten or automatically committed. */
                char expected_head[65];
                (void)snprintf(expected_head, sizeof(expected_head), "%s", copy->head);
                status = umi_publication_inspect(state, i);
                if (status != UMI_STATUS_OK) return status;
                if (state->dirty[i] || copy->changed_paths || strcmp(copy->head, expected_head) != 0 ||
                    strcmp(copy->remote_head, source->head) != 0)
                    return umi_publication_fail(state, i, UMI_STATUS_INVALID_STATE,
                        "A duplicate changed during publication; preserve it and review before retrying.");
                status = fast_forward(state, i, source->head);
                if (status != UMI_STATUS_OK) return status;
            }
            status = umi_publication_verify_links(state, i);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}
