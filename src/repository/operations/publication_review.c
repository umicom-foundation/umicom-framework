/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_review.c
 *
 * PURPOSE:
 *   Review an immutable staged tree and reject index changes before committing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <stdlib.h>
#include <string.h>

static int cancelled(const UmiPublicationState *state)
{
    return state->request && state->request->cancellation &&
        umi_cancellation_token_is_requested(state->request->cancellation);
}

static int object_id(const char *text)
{
    size_t length = strlen(text);
    return (length == 40U || length == 64U) &&
        strspn(text, "0123456789abcdefABCDEF") == length;
}

UmiStatus umi_publication_review_staged(UmiPublicationState *state,
    size_t index, const char *planned_tree)
{
    UmiPublicationCapture summary = {0}, patch = {0};
    UmiRepositoryPublicationEntry *entry = &state->report->entries[index];
    char current_tree[65];
    if (!state->review_staged) return UMI_STATUS_OK;
    if (cancelled(state))
        return umi_publication_fail(state, index, UMI_STATUS_CANCELLED,
            "Publication was cancelled before staged review.");
    if (!object_id(entry->head) || !object_id(planned_tree))
        return umi_publication_fail(state, index, UMI_STATUS_PARSE_ERROR,
            "Git returned an invalid object identity for staged review.");
    /* Diff immutable objects so the displayed summary and patch describe the
     * same snapshot even if another process writes the index during review.
     * Disable external diff/text conversion helpers and terminal colors. */
    const char *summary_args[] = {"diff", "--no-color", "--no-ext-diff", "--no-textconv",
        "--stat", entry->head, planned_tree, "--"};
    const char *patch_args[] = {"diff", "--no-color", "--no-ext-diff", "--no-textconv",
        "--patch", "--binary", entry->head, planned_tree, "--"};
    const char *tree_args[] = {"write-tree"};
    UmiStatus status = umi_publication_git(state, index, summary_args, 8U, 0, &summary);
    if (status == UMI_STATUS_OK)
        status = umi_publication_git(state, index, patch_args, 9U, 0, &patch);
    if (status == UMI_STATUS_OK &&
        (!summary.size || !patch.size || memchr(summary.bytes, '\0', summary.size) ||
            memchr(patch.bytes, '\0', patch.size)))
        status = umi_publication_fail(state, index, UMI_STATUS_PARSE_ERROR,
            "Git returned a malformed staged diff; no review was accepted.");
    if (status == UMI_STATUS_OK) {
        UmiRepositoryPublicationStagedReview review = {
            entry->path, entry->head, planned_tree, summary.bytes, summary.size, patch.bytes, patch.size
        };
        status = state->review_staged(&review, state->review_context);
        if (status != UMI_STATUS_OK)
            status = umi_publication_fail(state, index, status,
                "Staged review did not approve a new commit; the index and earlier completed actions are retained.");
    }
    free(summary.bytes); free(patch.bytes);
    if (status != UMI_STATUS_OK) return status;
    /* A GUI can cancel while its reviewer is open. Approval must not override
     * that cancellation when control returns to the worker. */
    if (cancelled(state))
        return umi_publication_fail(state, index, UMI_STATUS_CANCELLED,
            "Publication was cancelled during staged review; the index is retained.");
    status = umi_publication_text(state, index, tree_args, 1U, current_tree, sizeof(current_tree));
    if (status == UMI_STATUS_OK && strcmp(current_tree, planned_tree) != 0)
        status = umi_publication_fail(state, index, UMI_STATUS_INVALID_STATE,
            "The staged tree changed during review; no new commit was created for this checkout.");
    return status;
}
