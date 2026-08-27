/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_review_session.c
 *
 * PURPOSE:
 *   Verify provider-neutral inline review threads and approval policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/review_session.h"

int main(void)
{
    UmiVcsWorkbenchReviewSession session;
    assert(umi_vcs_workbench_review_session_begin(&session, "review-1") ==
           UMI_STATUS_OK);
    assert(umi_vcs_workbench_review_session_add_thread(
               &session, "thread-1", "src/a.c", 7U, 1,
               "comment-1", "reviewer", "Please explain this branch.", 10U) ==
           UMI_STATUS_OK);
    assert(session.thread_count == 1U);
    assert(session.unresolved_count == 1U);
    assert(umi_vcs_workbench_review_session_set_decision(
               &session, UMI_VCS_WORKBENCH_REVIEW_APPROVED) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_vcs_workbench_review_session_resolve_thread(
               &session, "thread-1", 1) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_review_session_set_decision(
               &session, UMI_VCS_WORKBENCH_REVIEW_APPROVED) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_review_session_comment_at(&session, 0U) != NULL);
    return 0;
}
