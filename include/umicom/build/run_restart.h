/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/run_restart.h
 *
 * PURPOSE:
 *   Restart an owned program only after terminal evidence and renewed project admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RUN_RESTART_H
#define UMICOM_BUILD_RUN_RESTART_H
#include "umicom/build/project_session.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildRunRestartSnapshot {
    uint64_t previous_operation_id;
    uint64_t submitted_operation_id;
    UmiStatus status;
    bool pending;
    char log_path[UMI_PATH_CAPACITY];
} UmiBuildRunRestartSnapshot;

/** Owner-thread inspection; no cancellation or file creation. Require a live
 * Run stage of an operation requested as Run, an unchanged admitted profile,
 * and current trust for the same project. Build/Test/Debug are never selected.
 * Settings identity does not prove executable bytes or inherited environment. */
UmiStatus UmiBuildRunRestartReview(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, const char *project_root, bool trusted);
/** Request cancellation and retain one restart intent. Duplicate requests return
 * BUSY. The owner must continue collecting results, then call Advance. If the
 * previous job had a log, the fresh name appends .restart-<next operation id>.
 * Existing files are never overwritten; creation is deferred until submission. */
UmiStatus UmiBuildRunRestartRequest(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, const char *project_root, bool trusted);
/** Recheck trust/project/settings each time. BUSY means still waiting for the
 * worker/queue or for consumed_results to equal the terminal phase count.
 * A mismatch or other failure clears the intent; no automatic retry occurs.
 * OK submits exactly one Run-current stage without saving or building source.
 * submitted is always initialized when non-NULL. This is not launch success.
 * The count is the host's acknowledgement that it retained completed results.
 * A concurrent Stop during admission remains requested on a just-queued task;
 * its terminal result, rather than the submission result, reports cancellation. */
UmiStatus UmiBuildRunRestartAdvance(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, const char *project_root, bool trusted,
    size_t consumed_results, bool *submitted);
/** Read copied owner-thread intent. Ordinary session Cancel revokes a pending
 * restart as well as requesting process cancellation. Destroy never relaunches. */
UmiStatus UmiBuildRunRestartRead(UmiBuildProjectSession *session,
    UmiBuildRunRestartSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
