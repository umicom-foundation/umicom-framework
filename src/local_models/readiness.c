/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/readiness.c
 *
 * PURPOSE:
 *   Observe endpoint readiness through bounded shared health probes without mutating model ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include "umicom/platform/threading.h"

UmiStatus UmiModelReadinessBudget(const UmiModelReadinessPlan *plan, uint32_t *outMillis)
{
    if (plan == NULL || outMillis == NULL || plan->port < 1024U ||
        plan->attempts < 1U || plan->attempts > 64U ||
        plan->interval_millis < 50U || plan->interval_millis > 5000U ||
        plan->timeout_millis < 100U || plan->timeout_millis > 60000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Widen before multiplying even though individual fields are bounded.
     * Future range changes must not silently wrap the admission calculation. */
    uint64_t budget = (uint64_t)plan->attempts * plan->timeout_millis +
        (uint64_t)(plan->attempts - 1U) * plan->interval_millis;
    if (budget > 60000U) return UMI_STATUS_INVALID_ARGUMENT;
    *outMillis = (uint32_t)budget;
    return UMI_STATUS_OK;
}

UmiStatus UmiModelReadinessPlanCreate(uint16_t port, uint32_t attempts,
    uint32_t intervalMillis, uint32_t timeoutMillis, UmiModelReadinessPlan *outPlan)
{
    if (outPlan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiModelReadinessPlan candidate = {0};
    candidate.port = port;
    candidate.attempts = attempts;
    candidate.interval_millis = intervalMillis;
    candidate.timeout_millis = timeoutMillis;
    uint32_t budget = 0U;
    UmiStatus status = UmiModelReadinessBudget(&candidate, &budget);
    if (status == UMI_STATUS_OK) *outPlan = candidate;
    return status;
}

static bool ReadinessPause(uint32_t milliseconds, const UmiAiWorkspaceCancellation *token)
{
    /* A GUI worker may be cancelled while the server is loading. Splitting
     * pauses keeps cancellation observable without a product-specific timer.
     * OS scheduling can delay wakeup; this is not a real-time guarantee. */
    while (milliseconds != 0U) {
        if (UmiAiWorkspaceCancellationIsRequested(token)) return false;
        uint32_t slice = milliseconds < 20U ? milliseconds : 20U;
        umi_thread_sleep_ms(slice);
        milliseconds -= slice;
    }
    return !UmiAiWorkspaceCancellationIsRequested(token);
}

UmiStatus UmiModelReadinessWait(const UmiModelReadinessPlan *plan, bool approved,
    const UmiAiWorkspaceCancellation *cancellation, UmiModelReadinessReport *outReport)
{
    uint32_t budget = 0U;
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiModelReadinessBudget(plan, &budget);
    if (status != UMI_STATUS_OK) return status;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (UmiAiWorkspaceCancellationIsRequested(cancellation)) return UMI_STATUS_CANCELLED;
    const UmiModelReadinessPlan captured = *plan;
    UmiModelReadinessReport observed = {0};
    observed.last_probe_status = UMI_STATUS_NOT_FOUND;
    /* Capture inputs and stage results locally. A failure after admission
     * replaces stale success with this call's evidence; a refusal does not. */
    status = UMI_STATUS_TIMEOUT;
    for (uint32_t attempt = 0U; attempt < captured.attempts; ++attempt) {
        if (UmiAiWorkspaceCancellationIsRequested(cancellation)) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        UmiAiLocalHealth health = {0};
        UmiStatus probe = UmiModelHealthProbe(captured.port, captured.timeout_millis,
            true, cancellation, &health);
        ++observed.attempts_completed;
        observed.last_probe_status = probe;
        observed.has_health = probe == UMI_STATUS_OK;
        observed.last_health = health;
        if (UmiAiWorkspaceCancellationIsRequested(cancellation)) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        if (probe == UMI_STATUS_OK && health.endpoint_reports_ready) {
            status = UMI_STATUS_OK;
            break;
        }
        if (probe != UMI_STATUS_OK && probe != UMI_STATUS_IO_ERROR && probe != UMI_STATUS_TIMEOUT) {
            status = probe;
            break;
        }
        if (attempt + 1U < captured.attempts &&
            !ReadinessPause(captured.interval_millis, cancellation)) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
    }
    *outReport = observed;
    return status;
}
