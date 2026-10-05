/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/log_capture.h
 * PURPOSE: Retain complete emitted build output independently of the bounded live display.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LOG_CAPTURE_H
#define UMICOM_BUILD_LOG_CAPTURE_H
#include "umicom/build/live_output.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBuildLogSnapshot {
    uint64_t operation_id;
    uint64_t bytes_received;
    bool enabled;
    bool streamed;
    bool capture_complete;
    UmiOutputFileSnapshot file;
} UmiBuildLogSnapshot;

/* Submit the usual trusted copied workflow, reserving a new absolute log path
 * before queuing work. Invalid paths, existing leaves and open failures refuse
 * submission; existing files and previous result/log evidence stay intact.
 * NULL selects the ordinary unlogged workflow. No parent directories are made.
 * The file concatenates raw merged stdout/stderr bytes from every attempted
 * phase, without added headers, separators or text conversion. Commands and
 * structured diagnostics remain in completed-result history.
 * Logging is synchronous on the worker: slow storage may delay execution and
 * cancellation. A later write/flush error is reported separately from the build
 * result; it never retries a build or cancels compilation automatically.
 * Keep the selected parent directory trusted; this is not a sandbox boundary. */
UmiStatus UmiBuildProjectSessionSubmitLogged(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, UmiBuildPhase phase, bool trusted, const char *log_path);
/* Read copied log progress without disk I/O. capture_complete means all bytes
 * emitted by an observed executor were written and the file closed successfully.
 * It does not imply build success, a complete workflow, or power-loss durability.
 * Legacy custom executors publish only their bounded final result and can never
 * claim complete capture. Cancellation still closes and retains the emitted log.
 * Files remain on disk after session destruction or the next submission; the
 * user owns retention. Logs may contain private paths or program output. */
UmiStatus UmiBuildProjectSessionReadLog(UmiBuildProjectSession *session, UmiBuildLogSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
