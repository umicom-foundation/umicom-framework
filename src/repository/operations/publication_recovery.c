/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_recovery.c
 * PURPOSE: Attach only clean, already-published leaf checkouts without rewriting files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "publication_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exit one from symbolic-ref means detached HEAD, not a failed Git process.
 * Keep it visible in the report so the owner can distinguish it from divergence. */
UmiStatus umi_publication_branch(UmiPublicationState *state, size_t index)
{
    const char *args[] = {"symbolic-ref", "--quiet", "--short", "HEAD"};
    UmiPublicationCapture capture;
    UmiRepositoryPublicationEntry *entry = &state->report->entries[index];
    UmiStatus status = umi_publication_git(state, index, args, 4U, 1, &capture);
    if (status != UMI_STATUS_OK) return status;
    entry->branch[0] = '\0';
    if (capture.exit_code == 0) {
        while (capture.size && (capture.bytes[capture.size - 1U] == '\n' ||
               capture.bytes[capture.size - 1U] == '\r')) --capture.size;
        if (capture.size == 0U || capture.size >= sizeof(entry->branch) ||
            memchr(capture.bytes, '\0', capture.size) != NULL) {
            free(capture.bytes);
            return umi_publication_fail(state, index, UMI_STATUS_PARSE_ERROR,
                "Git returned an invalid branch name.");
        }
        memcpy(entry->branch, capture.bytes, capture.size);
        entry->branch[capture.size] = '\0';
    }
    free(capture.bytes);
    return UMI_STATUS_OK;
}

/* A local main branch in another worktree belongs to that worktree's owner.
 * Never move it behind their working files, even when the destination is public. */
static UmiStatus main_not_checked_out(UmiPublicationState *state, size_t index)
{
    const char *args[] = {"worktree", "list", "--porcelain", "-z"};
    UmiPublicationCapture capture;
    UmiStatus status = umi_publication_git(state, index, args, 4U, 0, &capture);
    if (status != UMI_STATUS_OK) return status;
    size_t offset = 0U, worktrees = 0U;
    while (offset < capture.size) {
        char *record = capture.bytes + offset;
        char *end = memchr(record, '\0', capture.size - offset);
        if (end == NULL) { status = UMI_STATUS_PARSE_ERROR; break; }
        if (strncmp(record, "worktree ", 9U) == 0 && record[9] != '\0') ++worktrees;
        if (strcmp(record, "branch refs/heads/main") == 0) {
            status = UMI_STATUS_BUSY;
            break;
        }
        offset = (size_t)(end - capture.bytes) + 1U;
    }
    if (status == UMI_STATUS_OK && worktrees == 0U) status = UMI_STATUS_PARSE_ERROR;
    free(capture.bytes);
    if (status != UMI_STATUS_OK)
        return umi_publication_fail(state, index, status,
            "Cannot attach main: it is checked out elsewhere or its worktree inventory is invalid.");
    return UMI_STATUS_OK;
}

UmiStatus umi_publication_attachment_check(UmiPublicationState *state, size_t index)
{
    UmiRepositoryPublicationEntry *entry = &state->report->entries[index];
    state->attach_ready[index] = 0U;
    if (strcmp(entry->branch, "main") == 0) return UMI_STATUS_OK;
    if (entry->branch[0] != '\0')
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "This checkout is on another named branch; automatic branch replacement is not supported.");
    if (!state->attach_published_main)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Detached HEAD. Preview --attach-published-main for a clean, already-published leaf checkout; no files were changed.");
    if (!state->request->refresh || state->dirty[index] || entry->changed_paths ||
        entry->ahead || entry->behind || strcmp(entry->head, entry->remote_head) != 0)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Attachment requires clean files and HEAD exactly at freshly fetched origin/main; local work is preserved.");
    for (size_t e = 0U; e < state->edge_count; ++e)
        if (state->edges[e].parent == index)
            return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
                "Automatic attachment is limited to leaf repositories; parent dependency state needs review.");
    const char *main_ref[] = {"rev-parse", "--verify", "refs/heads/main"};
    UmiStatus status = umi_publication_text(state, index, main_ref, 3U,
        state->previous_main[index], sizeof(state->previous_main[index]));
    if (status != UMI_STATUS_OK) return status;
    const char *ancestor[] = {"merge-base", "--is-ancestor",
        state->previous_main[index], entry->head};
    UmiPublicationCapture capture;
    status = umi_publication_git(state, index, ancestor, 4U, 1, &capture);
    if (status != UMI_STATUS_OK) return status;
    int contained = capture.exit_code == 0;
    free(capture.bytes);
    if (!contained)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Local main contains history outside the published HEAD; preserve and review those commits.");
    status = main_not_checked_out(state, index);
    if (status != UMI_STATUS_OK) return status;
    state->attach_ready[index] = 1U;
    (void)snprintf(entry->detail, sizeof(entry->detail), "%s",
        "Eligible to attach the existing main branch to this published HEAD; working files and index remain unchanged.");
    return UMI_STATUS_OK;
}

UmiStatus umi_publication_attach_published(UmiPublicationState *state, size_t index)
{
    UmiRepositoryPublicationEntry *entry = &state->report->entries[index];
    char expected_head[65], expected_main[65];
    (void)snprintf(expected_head, sizeof(expected_head), "%s", entry->head);
    (void)snprintf(expected_main, sizeof(expected_main), "%s", state->previous_main[index]);
    /* Re-fetch and repeat every eligibility check under the publisher marker.
     * An earlier preview never authorises discarding changes made since then. */
    UmiStatus status = umi_publication_inspect(state, index);
    if (status != UMI_STATUS_OK) return status;
    if (!state->request->apply || !state->attach_ready[index] ||
        strcmp(entry->head, expected_head) != 0 ||
        strcmp(state->previous_main[index], expected_main) != 0)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "Repository state changed before attachment; no branch was attached.");
    UmiPublicationCapture capture;
    if (strcmp(expected_main, expected_head) != 0) {
        /* Compare-and-swap advances only the ancestor inspected above. The
         * reflog records the previous tip. This does not check out any files. */
        const char *advance[] = {"update-ref", "-m", "umicom: advance already-published main",
            "refs/heads/main", expected_head, expected_main};
        status = umi_publication_git(state, index, advance, 6U, 0, &capture);
        free(capture.bytes);
        if (status != UMI_STATUS_OK) return status;
        if (state->request->observer)
            state->request->observer(entry->path, "Advanced local main to its published descendant",
                state->request->observer_context);
    }
    /* Recheck both refs before binding HEAD. A failed binding leaves a normal
     * detached checkout and a published branch; it never restores old files. */
    const char *main_ref[] = {"rev-parse", "--verify", "refs/heads/main"};
    const char *head_ref[] = {"rev-parse", "--verify", "HEAD"};
    char value[65];
    status = umi_publication_branch(state, index);
    if (status != UMI_STATUS_OK) return status;
    if (entry->branch[0] != '\0') return umi_publication_fail(state, index,
        UMI_STATUS_INVALID_STATE, "HEAD was attached by another process; stopped without changing files.");
    for (unsigned ref = 0U; ref < 2U; ++ref) {
        status = umi_publication_text(state, index, ref ? head_ref : main_ref, 3U,
            value, sizeof(value));
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(value, expected_head) != 0)
            return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
                "A reference moved during recovery; the completed local branch advance is retained.");
    }
    status = main_not_checked_out(state, index);
    if (status != UMI_STATUS_OK) return status;
    const char *attach[] = {"symbolic-ref", "-m", "umicom: attach published main", "HEAD", "refs/heads/main"};
    status = umi_publication_git(state, index, attach, 5U, 0, &capture);
    free(capture.bytes);
    if (status != UMI_STATUS_OK) return status;
    ++state->report->updated_count;
    if (state->request->observer)
        state->request->observer(entry->path, "Attached published HEAD to main without rewriting files",
            state->request->observer_context);
    status = umi_publication_inspect(state, index);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(entry->head, expected_head) != 0 || strcmp(entry->branch, "main") != 0 ||
        state->dirty[index] || entry->ahead || entry->behind)
        return umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "State changed after attachment; review it before further publication.");
    return UMI_STATUS_OK;
}
