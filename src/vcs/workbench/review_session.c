/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/review_session.c
 *
 * PURPOSE:
 *   Implement bounded, provider-neutral inline review threads and decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/review_session.h"

#include <string.h>

static size_t thread_index(
    const UmiVcsWorkbenchReviewSession *session,
    const char *thread_id)
{
    size_t index;
    for (index = 0U; index < session->thread_count; ++index) {
        if (strcmp(session->threads[index].thread_id, thread_id) == 0) {
            return index;
        }
    }
    return session->thread_count;
}

static int comment_exists(
    const UmiVcsWorkbenchReviewSession *session,
    const char *comment_id)
{
    size_t index;
    for (index = 0U; index < session->comment_count; ++index) {
        if (strcmp(session->comments[index].comment_id, comment_id) == 0) {
            return 1;
        }
    }
    return 0;
}

void umi_vcs_workbench_review_session_init(
    UmiVcsWorkbenchReviewSession *session)
{
    if (session == NULL) return;
    (void)memset(session, 0, sizeof(*session));
    session->struct_size = (uint32_t)sizeof(*session);
    session->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    session->decision = UMI_VCS_WORKBENCH_REVIEW_PENDING;
    session->revision = 1U;
}

UmiStatus umi_vcs_workbench_review_session_begin(
    UmiVcsWorkbenchReviewSession *session,
    const char *session_id)
{
    UmiStatus status;

    if (session == NULL || session_id == NULL || session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_vcs_workbench_review_session_init(session);
    status = umi_vcs_workbench_copy_text(
        session->session_id, sizeof(session->session_id), session_id);
    if (status == UMI_STATUS_OK) {
        session->ready = 1;
        session->revision += 1U;
    }
    return status;
}

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

    if (session == NULL || !session->ready || thread_id == NULL ||
        comment_id == NULL || comment_id[0] == '\0' || author == NULL ||
        author[0] == '\0' || body == NULL || body[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    owner = thread_index(session, thread_id);
    if (owner >= session->thread_count) return UMI_STATUS_NOT_FOUND;
    if (comment_exists(session, comment_id)) return UMI_STATUS_ALREADY_EXISTS;
    if (session->comment_count >= UMI_VCS_WORKBENCH_MAX_REVIEW_COMMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    comment = &session->comments[session->comment_count];
    (void)memset(comment, 0, sizeof(*comment));
    comment->struct_size = (uint32_t)sizeof(*comment);
    comment->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    status = umi_vcs_workbench_copy_text(
        comment->comment_id, sizeof(comment->comment_id), comment_id);
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        comment->thread_id, sizeof(comment->thread_id), thread_id);
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        comment->author, sizeof(comment->author), author);
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        comment->body, sizeof(comment->body), body);
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

    if (session == NULL || !session->ready || thread_id == NULL ||
        thread_id[0] == '\0' || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (thread_index(session, thread_id) < session->thread_count) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
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
    if (status == UMI_STATUS_OK) status = umi_vcs_workbench_copy_text(
        thread->path, sizeof(thread->path), path);
    if (status != UMI_STATUS_OK) {
        (void)memset(thread, 0, sizeof(*thread));
        return status;
    }
    session->thread_count += 1U;
    session->unresolved_count += 1U;
    status = umi_vcs_workbench_review_session_add_comment(
        session, thread_id, comment_id, author, body, created_at_ms);
    if (status != UMI_STATUS_OK) {
        session->thread_count -= 1U;
        session->unresolved_count -= 1U;
        (void)memset(thread, 0, sizeof(*thread));
        return status;
    }
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_review_session_resolve_thread(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    int resolved)
{
    size_t index;
    int target;

    if (session == NULL || !session->ready || thread_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = thread_index(session, thread_id);
    if (index >= session->thread_count) return UMI_STATUS_NOT_FOUND;
    target = resolved != 0;
    if (session->threads[index].resolved != target) {
        session->threads[index].resolved = target;
        if (target) session->unresolved_count -= 1U;
        else session->unresolved_count += 1U;
        session->threads[index].revision += 1U;
        session->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_review_session_set_decision(
    UmiVcsWorkbenchReviewSession *session,
    UmiVcsWorkbenchReviewDecision decision)
{
    if (session == NULL || !session->ready ||
        decision < UMI_VCS_WORKBENCH_REVIEW_PENDING ||
        decision > UMI_VCS_WORKBENCH_REVIEW_CHANGES_REQUESTED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (decision == UMI_VCS_WORKBENCH_REVIEW_APPROVED &&
        session->unresolved_count > 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->decision = decision;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiVcsWorkbenchReviewThread *
umi_vcs_workbench_review_session_thread_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index)
{
    return session != NULL && index < session->thread_count
        ? &session->threads[index] : NULL;
}

const UmiVcsWorkbenchReviewComment *
umi_vcs_workbench_review_session_comment_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index)
{
    return session != NULL && index < session->comment_count
        ? &session->comments[index] : NULL;
}
