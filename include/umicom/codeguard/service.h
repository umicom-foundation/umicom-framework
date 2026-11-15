/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/service.h
 *
 * PURPOSE:
 *   Expose a reusable CodeGuard service for Studio, Helix and other Umicom
 *   products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_SERVICE_H
#define UMICOM_CODEGUARD_SERVICE_H
#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/summary.h"
#include "umicom/codeguard/job.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard service data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardService UmiCodeGuardService;
/**
 * Initialise codeguard service from caller-provided values so later operations receive a
 * known state.
 */
/* Copy a nonempty root of at most 1023 bytes. Overlong input is rejected,
 * never truncated. A valid out pointer is cleared on failure. Relative roots
 * retain their existing meaning: resolution occurs in the scan caller's
 * current directory; use an absolute root for a stable workspace identity. */
UmiStatus umi_codeguard_service_create(const char *root, UmiCodeGuardService **out_service);
/**
 * Release or reset state held by codeguard service so the same storage can be reused
 * safely.
 */
void umi_codeguard_service_destroy(UmiCodeGuardService *service);
/**
 * Provide the codeguard service scan operation used by this module and its client
 * applications.
 */
/* NULL profile selects existing defaults. Success replaces the findings;
 * failure preserves the last complete result and summary, including borrows.
 * A result is not a clean bill of health: callers must check this return
 * status and apply their chosen severity gate. Access must be serialized;
 * this ordinary entry point does not observe a token or create a worker.
 * The cancellable counterpart below shares the same ownership contract. */
UmiStatus umi_codeguard_service_scan(UmiCodeGuardService *service, const UmiCodeGuardProfile *profile);
/**
 * Provide the codeguard service result operation used by this module and its client
 * applications.
 */
/* Borrow the stable result object until service destruction. Individual
 * finding pointers survive failed scans, but not successful replacement. */
const UmiCodeGuardResult *umi_codeguard_service_result(const UmiCodeGuardService *service);
/**
 * Provide the codeguard service summary operation used by this module and its client
 * applications.
 */
UmiCodeGuardSummary umi_codeguard_service_summary(const UmiCodeGuardService *service);

/* Historical contract: this synchronous service did not add cancellation
 * or a worker thread. The new optional token adds cooperative observation;
 * it retains the original externally serialized synchronous execution. */
/* The service stages findings and commits only after the final cancellation
 * check. UMI_STATUS_CANCELLED preserves the prior complete result, summary
 * and finding borrows. NULL token is equivalent to service_scan. A request
 * racing after the final check may arrive after successful publication.
 * Serialize service access and retain the token until return; only requesting
 * the token is safe concurrently. This call does not schedule a worker. */
UmiStatus umi_codeguard_service_scan_cancellable(UmiCodeGuardService *service,
    const UmiCodeGuardProfile *profile, const UmiCancellationToken *cancellation);
/* Create an independent job from this service's root; NULL profile uses defaults.
 * A valid output is cleared on failure. Relative roots are resolved now. No
 * scan starts. The job remains valid after service destruction. */
UmiStatus umi_codeguard_service_create_job(UmiCodeGuardService *service,
    const UmiCodeGuardProfile *profile, UmiCodeGuardJob **out_job);

/* Explicitly adopt one successful terminal job on its creating thread, while
 * serializing all service access. Match the service's current resolved root
 * before taking the result. A mismatch returns INVALID_STATE without consuming
 * it. BUSY, cancellation and scan failure preserve prior finding borrows.
 * Success keeps the result object address but invalidates old finding borrows.
 * The caller still owns and destroys the job. There is no newest-job ordering:
 * the caller chooses which completed result to adopt. Absolute service roots
 * avoid a change of current directory changing relative-root identity. */
UmiStatus umi_codeguard_service_adopt_job(UmiCodeGuardService *service, UmiCodeGuardJob *job);

#ifdef __cplusplus
}
#endif
#endif
