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

/**
 * Represent the vcs workbench review thread data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the vcs workbench review comment data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkbenchReviewComment {
    uint32_t struct_size;
    uint32_t api_version;
    char comment_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    char thread_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    char author[UMI_VCS_WORKBENCH_LABEL_CAPACITY];
    char body[UMI_VCS_WORKBENCH_TEXT_CAPACITY];
    uint64_t created_at_ms;
} UmiVcsWorkbenchReviewComment;

/**
 * Represent the vcs workbench review session data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise vcs workbench review session from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_workbench_review_session_init(
    UmiVcsWorkbenchReviewSession *session);
/**
 * Provide the vcs workbench review session begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_review_session_begin(
    UmiVcsWorkbenchReviewSession *session,
    const char *session_id);
/**
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
    uint64_t created_at_ms);
/**
 * Provide the vcs workbench review session add comment operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_review_session_add_comment(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    const char *comment_id,
    const char *author,
    const char *body,
    uint64_t created_at_ms);
/**
 * Provide the vcs workbench review session resolve thread operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workbench_review_session_resolve_thread(
    UmiVcsWorkbenchReviewSession *session,
    const char *thread_id,
    int resolved);
/**
 * Provide the vcs workbench review session set decision operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_review_session_set_decision(
    UmiVcsWorkbenchReviewSession *session,
    UmiVcsWorkbenchReviewDecision decision);
/**
 * Find vcs workbench review session thread while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiVcsWorkbenchReviewThread *
umi_vcs_workbench_review_session_thread_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index);
/**
 * Find vcs workbench review session comment while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiVcsWorkbenchReviewComment *
umi_vcs_workbench_review_session_comment_at(
    const UmiVcsWorkbenchReviewSession *session,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_REVIEW_SESSION_H */
