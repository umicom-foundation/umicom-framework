/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/live_output.h
 * PURPOSE: Publish bounded, copied build output while a project phase is running.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LIVE_OUTPUT_H
#define UMICOM_BUILD_LIVE_OUTPUT_H
#include "umicom/build/project_session.h"
#include "umicom/platform/process.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_LIVE_OUTPUT_CAPACITY 65536U

/* A copied tail of the current phase, independent of the completed-result
 * history. length counts raw bytes, including embedded NUL bytes. bytes[length]
 * is an extra terminator; consumers must use length rather than strlen().
 * Chunks can split UTF-8 characters. Text frontends should repair only their
 * display copy, retaining the byte counts and truncation notice below.
 * Allocate this record on the heap in frontends with small thread stacks. */
typedef struct UmiBuildOutputSnapshot {
    uint64_t operation_id;
    uint64_t revision;
    uint64_t total_bytes;
    size_t phase_index;
    UmiBuildPhase phase;
    UmiStatus status;
    size_t length;
    bool truncated;
    bool phase_complete;
    bool streamed;
    bool counters_saturated;
    char bytes[UMI_BUILD_LIVE_OUTPUT_CAPACITY];
} UmiBuildOutputSnapshot;

/* The observer runs synchronously on the calling thread with borrowed bytes.
 * It must return promptly and must not destroy or re-enter the runner. Both
 * output streams are merged. A NULL observer preserves the original run API.
 * Final diagnostics, history, cancellation and process-tree ownership use the
 * same runner path regardless of whether an observer is attached. */
UmiStatus UmiBuildRunnerRunObserved(UmiBuildRunner *runner, UmiBuildPhase phase,
    UmiProcessOutputObserver observer, void *context, UmiBuildResult *out_result);

/* Optional executor for hosts providing their own build transport. Emit bytes
 * synchronously before returning, honour cancellation, and do not retain the
 * observer/context. The session copies bytes before each callback returns.
 * Config.context is the executor context. Config.execute must be NULL when an
 * observed executor is supplied; this avoids ambiguous ownership. */
typedef UmiStatus (*UmiBuildProjectExecuteObserved)(const UmiBuildProfile *profile,
    UmiBuildPhase phase, UmiCancellationToken *cancellation,
    UmiProcessOutputObserver observer, void *observer_context,
    UmiBuildResult *out_result, void *context);
UmiStatus UmiBuildProjectSessionCreateObserved(const UmiBuildProjectSessionConfig *config,
    UmiBuildProjectExecuteObserved execute, UmiBuildProjectSession **out_session);

/* Read without waiting for a compiler or calling application callbacks. The
 * session mutex protects one coherent copy; the caller owns it after return.
 * operation_id == 0 means no submitted job. A new phase starts an empty tail.
 * Slow readers may miss a whole phase; completed results remain available via
 * result_at(). The legacy custom executor publishes final output only, with
 * streamed == false. Truncation describes this tail, not the result parser.
 * revision increases within an operation. If counters_saturated is set,
 * refresh on every poll instead of relying on revision equality. This read may
 * overlap the worker, but must not overlap destruction of the session. */
UmiStatus UmiBuildProjectSessionReadOutput(UmiBuildProjectSession *session,
    UmiBuildOutputSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
