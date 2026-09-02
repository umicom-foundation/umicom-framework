/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/review_session.c
 *
 * PURPOSE:
 *   Implement bounded, provider-neutral inline review threads and decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/review_session.h"

#include <string.h>

/* Provide the thread index operation used by this module and its client applications. */
static size_t thread_index(
    const UmiVcsWorkbenchReviewSession *session,
    const char *thread_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->thread_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(session->threads[index].thread_id, thread_id) == 0) {
            return index;
        }
    }
    return session->thread_count;
}

/* Provide the comment exists operation used by this module and its client applications. */
static int comment_exists(
    const UmiVcsWorkbenchReviewSession *session,
    const char *comment_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->comment_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(session->comments[index].comment_id, comment_id) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Initialise vcs workbench review session from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_workbench_review_session_init(
    UmiVcsWorkbenchReviewSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    (void)memset(session, 0, sizeof(*session));
    session->struct_size = (uint32_t)sizeof(*session);
    session->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    session->decision = UMI_VCS_WORKBENCH_REVIEW_PENDING;
    session->revision = 1U;
}

/*
 * Provide the vcs workbench review session begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_review_session_begin(
    UmiVcsWorkbenchReviewSession *session,
    const char *session_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session_id == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_vcs_workbench_review_session_init(session);
    status = umi_vcs_workbench_copy_text(
        session->session_id, sizeof(session->session_id), session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->ready = 1;
        session->revision += 1U;
    }
    return status;
}

/*
 * Provide the vcs workbench review session add comment operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_review_session_add_comment(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    const char *comment_id,
    const char *author,
    const char *body,
    uint64_t created_at_ms)
{
    UmiVcsWorkbenchReviewComment *comment;
    size_t owner;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->ready || thread_id == NULL ||
        comment_id == NULL || comment_id[0] == '\0' || author == NULL ||
        author[0] == '\0' || body == NULL || body[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    owner = thread_index(session, thread_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (owner >= session->thread_count) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (comment_exists(session, comment_id)) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->comment_count >= UMI_VCS_WORKBENCH_MAX_REVIEW_COMMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    comment = &session->comments[session->comment_count];
    (void)memset(comment, 0, sizeof(*comment));
    comment->struct_size = (uint32_t)sizeof(*comment);
    comment->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    status = umi_vcs_workbench_copy_text(
        comment->comment_id, sizeof(comment->comment_id), comment_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        comment->thread_id, sizeof(comment->thread_id), thread_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        comment->author, sizeof(comment->author), author);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        comment->body, sizeof(comment->body), body);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(comment, 0, sizeof(*comment));
        return status;
    }
    comment->created_at_ms = created_at_ms;
    session->comment_count += 1U;
    session->threads[owner].comment_count += 1U;
    session->threads[owner].revision += 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workbench review session add thread operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_review_session_add_thread(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    const char *path,
    size_t line_number,
    int right_side,
    const char *comment_id,
    const char *author,
    const char *body,
    uint64_t created_at_ms)
{
    UmiVcsWorkbenchReviewThread *thread;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->ready || thread_id == NULL ||
        thread_id[0] == '\0' || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (thread_index(session, thread_id) < session->thread_count) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->thread_count >= UMI_VCS_WORKBENCH_MAX_REVIEW_THREADS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    thread = &session->threads[session->thread_count];
    (void)memset(thread, 0, sizeof(*thread));
    thread->struct_size = (uint32_t)sizeof(*thread);
    thread->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    thread->line_number = line_number;
    thread->right_side = right_side != 0;
    thread->revision = 1U;
    status = umi_vcs_workbench_copy_text(
        thread->thread_id, sizeof(thread->thread_id), thread_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        thread->path, sizeof(thread->path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(thread, 0, sizeof(*thread));
        return status;
    }
    session->thread_count += 1U;
    session->unresolved_count += 1U;
    status = umi_vcs_workbench_review_session_add_comment(
        session, thread_id, comment_id, author, body, created_at_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->thread_count -= 1U;
        session->unresolved_count -= 1U;
        (void)memset(thread, 0, sizeof(*thread));
        return status;
    }
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workbench review session resolve thread operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workbench_review_session_resolve_thread(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    int resolved)
{
    size_t index;
    int target;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->ready || thread_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = thread_index(session, thread_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= session->thread_count) return UMI_STATUS_NOT_FOUND;
    target = resolved != 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->threads[index].resolved != target) {
        session->threads[index].resolved = target;
        /* Configure the optional target only when its feature has created it. */
        if (target) session->unresolved_count -= 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else session->unresolved_count += 1U;
        session->threads[index].revision += 1U;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workbench review session set decision operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_review_session_set_decision(
    UmiVcsWorkbenchReviewSession *session,
    UmiVcsWorkbenchReviewDecision decision)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || !session->ready ||
        decision < UMI_VCS_WORKBENCH_REVIEW_PENDING ||
        decision > UMI_VCS_WORKBENCH_REVIEW_CHANGES_REQUESTED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (decision == UMI_VCS_WORKBENCH_REVIEW_APPROVED &&
        session->unresolved_count > 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->decision = decision;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workbench review session thread while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiVcsWorkbenchReviewThread *
umi_vcs_workbench_review_session_thread_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index)
{
    return session != NULL && index < session->thread_count
        ? &session->threads[index] : NULL;
}

/*
 * Find vcs workbench review session comment while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiVcsWorkbenchReviewComment *
umi_vcs_workbench_review_session_comment_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index)
{
    return session != NULL && index < session->comment_count
        ? &session->comments[index] : NULL;
}
