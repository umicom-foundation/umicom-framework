/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/project_session.h
 *
 * PURPOSE:
 *   Run a copied project build profile in the Framework worker queue while
 *   frontends poll completed results on their owning thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_PROJECT_SESSION_H
#define UMICOM_BUILD_PROJECT_SESSION_H

#include <stdbool.h>
#include "umicom/build/runner.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_PROJECT_SESSION_MAX_PHASES 3U

typedef struct UmiBuildProjectSession UmiBuildProjectSession;

/** Optional worker-side executor for controlled hosts and inert native tests.
 * It receives borrowed copied input and must honour cancellation. It must not
 * call session destruction or access GTK/document/command-registry objects. */
typedef UmiStatus (*UmiBuildProjectSessionExecute)(const UmiBuildProfile *profile,
    UmiBuildPhase phase, UmiCancellationToken *cancellation,
    UmiBuildResult *out_result, void *context);

typedef struct UmiBuildProjectSessionConfig {
    UmiClock *clock;
    UmiBuildProjectSessionExecute execute;
    void *context;
} UmiBuildProjectSessionConfig;

typedef struct UmiBuildProjectSessionSnapshot {
    uint64_t operation_id;
    UmiBuildPhase requested_phase;
    UmiBuildPhase current_phase;
    UmiStatus status;
    size_t completed_phase_count;
    bool active;
    bool cancellation_requested;
} UmiBuildProjectSessionSnapshot;

/** The clock/executor context is borrowed until destroy returns. No process
 * runs during creation. Each session owns one bounded Framework worker. */
UmiStatus umi_build_project_session_create(const UmiBuildProjectSessionConfig *config,
    UmiBuildProjectSession **out_session);
/** Owner-thread shutdown requests cancellation and joins its worker before
 * releasing inputs/results. Never destroy from an executor callback. */
void umi_build_project_session_destroy(UmiBuildProjectSession *session);
/** Copy a validated profile. Trusted must reflect the owner's explicit current
 * workspace trust decision. Build runs Configure then Build; Run runs Configure,
 * Build then the configured program. Test/Install run Configure, Build-all, then
 * Test/Install. Configure and Clean run once. No shell is used.
 * A nonempty Run program is required; no default executable is guessed. */
UmiStatus umi_build_project_session_submit(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, UmiBuildPhase phase, bool trusted);
/** Poll copied state/results; calls do not invoke owner callbacks or wait for
 * compiler completion. A result is readable only after its phase completes. */
UmiStatus umi_build_project_session_snapshot(UmiBuildProjectSession *session,
    UmiBuildProjectSessionSnapshot *out_snapshot);
UmiStatus umi_build_project_session_result_at(UmiBuildProjectSession *session,
    size_t index, UmiBuildResult *out_result);
/** Cooperative cancellation reaches the child-process runner immediately. */
void umi_build_project_session_cancel(UmiBuildProjectSession *session);

#ifdef __cplusplus
}
#endif
#endif
