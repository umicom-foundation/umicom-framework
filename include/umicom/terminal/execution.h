/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/execution.h
 * PURPOSE: Run one captured terminal command without blocking its application thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_EXECUTION_H
#define UMICOM_TERMINAL_EXECUTION_H
#include "umicom/platform/process_supervisor.h"
#include "umicom/terminal/session.h"
#ifdef __cplusplus
extern "C"
{
#endif

    /** A copied view of one command. Output is a bounded tail, not a complete log.
 * pending remains true until the owning thread polls and publishes completion.
 * A completed result remains readable until another command is accepted. */
    typedef struct UmiTerminalExecutionSnapshot
    {
        UmiProcessJobSnapshot process;
        int pending;
        int stop_requested;
    } UmiTerminalExecutionSnapshot;

    /** Submit a copied command, directory and environment to the borrowed supervisor.
 * Call on the session's owning thread. The supervisor must outlive this job.
 * Existing synchronous commands remain available; overlapping commands are refused.
 * This captures output from a single process tree; it does not provide terminal
 * emulation or interactive standard input. timeout_ms zero means no deadline. */
    UmiStatus UmiTerminalSessionStartJob(UmiTerminalSession *session,
                                         UmiProcessSupervisor *supervisor,
                                         const UmiTerminalCommand *command,
                                         const char *display_text, uint32_t timeout_ms);

    /** Copy current output and publish completed output into the session exactly once.
 * Call on the owning thread. Poll does not wait for a running process. It joins
 * only a worker which has already published a terminal state, then frees its slot.
 * Returned OK means the snapshot was read; inspect process.state and exit_code
 * to determine whether the command succeeded. */
    UmiStatus UmiTerminalSessionPollJob(UmiTerminalSession *session,
                                        UmiTerminalExecutionSnapshot *out_snapshot);

    /** Request cancellation of this session's command and ordinary child processes.
 * Stop is idempotent while the session still owns the job. Poll to observe
 * completion; a request racing normal completion may still finish successfully. */
    UmiStatus UmiTerminalSessionStopJob(UmiTerminalSession *session);

    /** Wait for this job during explicit teardown or a non-GUI workflow, then publish
 * its final snapshot and release the supervisor slot. Zero waits without a deadline.
 * Normal interface refresh must use Poll instead. */
    UmiStatus UmiTerminalSessionWaitJob(UmiTerminalSession *session, uint32_t timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
