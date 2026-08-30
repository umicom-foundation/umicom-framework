/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/review_session.h
 *
 * PURPOSE:
 *   Maintain provider-neutral inline review threads and decisions attached to
 *   a comparison session. Remote forges remain adapters outside this model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_REVIEW_SESSION_H
#define UMICOM_VCS_WORKBENCH_REVIEW_SESSION_H

#include "umicom/vcs/advanced/types.h"
#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsWorkbenchReviewThread {
    uint32_t struct_size;
    uint32_t api_version;
    char thread_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    size_t line_number;
    size_t comment_count;
    int right_side;
    int resolved;
    uint64_t revision;
} UmiVcsWorkbenchReviewThread;

typedef struct UmiVcsWorkbenchReviewComment {
    uint32_t struct_size;
    uint32_t api_version;
    char comment_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    char thread_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    char author[UMI_VCS_WORKBENCH_LABEL_CAPACITY];
    char body[UMI_VCS_WORKBENCH_TEXT_CAPACITY];
    uint64_t created_at_ms;
} UmiVcsWorkbenchReviewComment;

typedef struct UmiVcsWorkbenchReviewSession {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    UmiVcsWorkbenchReviewThread threads[
        UMI_VCS_WORKBENCH_MAX_REVIEW_THREADS];
    UmiVcsWorkbenchReviewComment comments[
        UMI_VCS_WORKBENCH_MAX_REVIEW_COMMENTS];
    size_t thread_count;
    size_t comment_count;
    size_t unresolved_count;
    UmiVcsWorkbenchReviewDecision decision;
    int ready;
    uint64_t revision;
} UmiVcsWorkbenchReviewSession;

void umi_vcs_workbench_review_session_init(
    UmiVcsWorkbenchReviewSession *session);
UmiStatus umi_vcs_workbench_review_session_begin(
    UmiVcsWorkbenchReviewSession *session,
    const char *session_id);
UmiStatus umi_vcs_workbench_review_session_add_thread(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    const char *path,
    size_t line_number,
    int right_side,
    const char *comment_id,
    const char *author,
    const char *body,
    uint64_t created_at_ms);
UmiStatus umi_vcs_workbench_review_session_add_comment(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    const char *comment_id,
    const char *author,
    const char *body,
    uint64_t created_at_ms);
UmiStatus umi_vcs_workbench_review_session_resolve_thread(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    int resolved);
UmiStatus umi_vcs_workbench_review_session_set_decision(
    UmiVcsWorkbenchReviewSession *session,
    UmiVcsWorkbenchReviewDecision decision);
const UmiVcsWorkbenchReviewThread *
umi_vcs_workbench_review_session_thread_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index);
const UmiVcsWorkbenchReviewComment *
umi_vcs_workbench_review_session_comment_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_REVIEW_SESSION_H */
