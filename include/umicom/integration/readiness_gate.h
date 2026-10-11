/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/readiness_gate.h
 *
 * PURPOSE:
 *   Bind a suite member's readiness observation to an exact supervised process
 *   instance and a one-use transport challenge. The suite runtime remains the
 *   sole owner of member lifecycle and readiness state.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_READINESS_GATE_H
#define UMICOM_INTEGRATION_READINESS_GATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/integration/suite_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The host MUST use a cryptographically secure operating-system random source
 * for every challenge. Neither process tokens nor process IDs are secrets.
 * This is a transport-neutral policy gate, NOT an IPC or authentication server.
 * The host must first verify the transport peer's identity and access rights. */
#define UMI_INTEGRATION_READINESS_CHALLENGE_BYTES 32U
#define UMI_INTEGRATION_READINESS_MAX_WAIT_MS 300000U

typedef struct UmiIntegrationReadinessGate UmiIntegrationReadinessGate;

/* A snapshot deliberately omits the challenge. The process token is an opaque
 * identifier, NOT an authentication credential. Fields remain owned by caller. */
typedef struct UmiIntegrationReadinessSnapshot {
    uint32_t structureSize;
    char applicationId[UMI_INTEGRATION_ID_CAPACITY];
    uint64_t processToken;
    uint64_t processGeneration;
    uint64_t expiresAtMs;
    bool pending;
    bool confirmed;
} UmiIntegrationReadinessSnapshot;

/* Borrow the existing, fully prepared suite runtime. The caller owns the
 * runtime and serialises ALL mutations, including ordinary suite functions.
 * Recreate the gate after calling suite_runtime_prepare or changing membership;
 * never persist, shallow-copy or transfer this opaque instance between threads.
 * No process, socket, secret store, timer or new application registry is made. */
UmiStatus UmiIntegrationReadinessGateCreate(
    UmiIntegrationSuiteRuntime *runtime,
    UmiIntegrationReadinessGate **outGate);
/**
 * @brief Release the gate and erase its retained challenge bytes.
 * @param gate Gate owned by the caller, or NULL.
 * @details The borrowed suite runtime remains owned by the caller and is not
 * freed or stopped. Call this before releasing that runtime. A replacement
 * transport can reuse the runtime only after arranging its own peer checks.
 */
void UmiIntegrationReadinessGateDestroy(UmiIntegrationReadinessGate *gate);

/* The owning Desk/supervisor MUST validate process_token and generation against
 * its current tracked process BEFORE Arm. The CSPRNG challenge is borrowed only
 * during the call and copied into the private gate. Each generation must be
 * greater than that member's previous generation in this gate. The expiry is
 * computed using caller-supplied monotonic milliseconds; zero/unbounded waits
 * and clock arithmetic overflow are refused without mutating suite state. */
UmiStatus UmiIntegrationReadinessGateArm(
    UmiIntegrationReadinessGate *gate, const char *applicationId,
    uint64_t processToken, uint64_t processGeneration,
    const uint8_t challenge[UMI_INTEGRATION_READINESS_CHALLENGE_BYTES],
    uint64_t nowMs, uint32_t waitMs);

/* Only the trusted transport host may set peerVerifiedByTransport=true after
 * validating operating-system peer credentials or a mutually authenticated
 * secure transport binding to the tracked process instance. Echoing the
 * challenge WITHOUT that peer check is NOT authentication. Expired, replayed,
 * mismatched, or unauthorised confirmations leave suite state unchanged. */
UmiStatus UmiIntegrationReadinessGateConfirm(
    UmiIntegrationReadinessGate *gate, const char *applicationId,
    uint64_t processToken, uint64_t processGeneration,
    const uint8_t response[UMI_INTEGRATION_READINESS_CHALLENGE_BYTES],
    uint64_t nowMs, bool peerVerifiedByTransport);

/* After the authoritative Desk process owner accepts the EXACT process token
 * and generation as current, a confirmed exit can be reflected in the suite.
 * This operation cannot kill/restart children or accept stale process exits. */
UmiStatus UmiIntegrationReadinessGateExit(
    UmiIntegrationReadinessGate *gate, const char *applicationId,
    uint64_t processToken, uint64_t processGeneration,
    int exitCode);

/* A gate-specific readiness decision is stricter than the legacy suite
 * usability check: ALL required members must have passed this gate's exact
 * process-instance proof. Hosts enforcing this policy must use this result
 * rather than treating registry RUNNING evidence as authenticated readiness. */
bool UmiIntegrationReadinessGateIsReady(
    const UmiIntegrationReadinessGate *gate);

/* No secret challenge bytes are returned. This remains a local owner-thread
 * snapshot, not evidence that the operating system or an external server ran. */
UmiStatus UmiIntegrationReadinessGateSnapshot(
    const UmiIntegrationReadinessGate *gate, const char *applicationId,
    UmiIntegrationReadinessSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
