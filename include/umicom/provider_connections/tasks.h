/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/tasks.h
 *
 * PURPOSE:
 *   Represent remote submission uncertainty and confirmed recovery plans without performing provider requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PROVIDER_CONNECTIONS_TASKS_H
#define UMICOM_PROVIDER_CONNECTIONS_TASKS_H
#include "umicom/provider_connections/connections.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PROVIDER_TASK_SCHEMA 1U
#define UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY 129U
typedef enum UmiProviderTaskPhase {
    UMI_PROVIDER_TASK_PREPARED=1,
    UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN=2,
    UMI_PROVIDER_TASK_SUBMITTED=3,
    UMI_PROVIDER_TASK_RUNNING=4,
    UMI_PROVIDER_TASK_SUCCEEDED=5,
    UMI_PROVIDER_TASK_FAILED=6,
    UMI_PROVIDER_TASK_OUTPUT_STORED=7
} UmiProviderTaskPhase;
typedef enum UmiProviderTaskEvent {
    UMI_PROVIDER_TASK_BEGIN_SUBMISSION=1,
    UMI_PROVIDER_TASK_OBSERVE_RUNNING=2,
    UMI_PROVIDER_TASK_OBSERVE_SUCCESS=3,
    UMI_PROVIDER_TASK_OBSERVE_FAILURE=4
} UmiProviderTaskEvent;
typedef enum UmiProviderTaskRecoveryAction {
    UMI_PROVIDER_TASK_POLL=1,
    UMI_PROVIDER_TASK_FETCH_OUTPUT=2
} UmiProviderTaskRecoveryAction;
/* IDs use the connection service's bounded lower-case identifier grammar.
 * operation_id names an adapter-reviewed operation, never an arbitrary URL.
 * request_sha256 binds the actual reviewed request bytes; it stores no prompt. */
typedef struct UmiProviderTaskContext {
    char application_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char profile_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char local_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char operation_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    unsigned char request_sha256[UMI_SHA256_BYTES];
} UmiProviderTaskContext;
typedef struct UmiProviderTaskReceipt {
    uint32_t schema;
    uint64_t revision;
    UmiProviderTaskContext context;
    char connection_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char provider_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    uint64_t connection_revision;
    unsigned char connection_fingerprint[UMI_SHA256_BYTES];
    UmiProviderTaskPhase phase;
    char remote_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
    uint64_t output_bytes;
    unsigned char output_sha256[UMI_SHA256_BYTES];
} UmiProviderTaskReceipt;
/* Pure value operations; no store, vault, network, process or worker lease.
 * The host serializes access and persists staged changes using compare-revision
 * storage. Preserve every output byte on failure; in-place receipt updates work.
 * Receipt validation checks consistency, not authenticity of supplied evidence. */
UmiStatus UmiProviderTaskValidate(const UmiProviderTaskReceipt *receipt);
/* Requires an enabled validated connection and a nonzero saved scope revision.
 * Initial receipt revision is one. Reusing local_id needs store-level refusal. */
UmiStatus UmiProviderTaskPrepare(const UmiProviderTaskContext *context,
    const UmiProviderConnection *connection,uint64_t connection_revision,
    UmiProviderTaskReceipt *out_receipt);
/* BEGIN_SUBMISSION changes PREPARED to SUBMISSION_UNCERTAIN. The host MUST
 * durably save this value before sending the creation request. Unknown outcome
 * cannot be rewound or automatically resubmitted. Poll observations require a
 * confirmed task. Repeated matching observations are no-ops after revision
 * comparison. Local timeout/cancel is not OBSERVE_FAILURE: only the provider's
 * confirmed terminal failure belongs there. */
UmiStatus UmiProviderTaskAdvance(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision,UmiProviderTaskEvent event,UmiProviderTaskReceipt *out_receipt);
/* Confirm only from SUBMISSION_UNCERTAIN; repeating the same confirmed ID in
 * SUBMITTED is a no-op. IDs are 1..128 ASCII letters/digits plus '.', '_', '-',
 * ':' after an alphanumeric first byte. No URL, credential or output link.
 * Save the confirmed ID before honoring a local stop-waiting request. */
UmiStatus UmiProviderTaskConfirm(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision,const char *remote_id,UmiProviderTaskReceipt *out_receipt);
/* Record positive byte count and observed digest only after the host has stored
 * and checked an output. SUCCEEDED means provider success, not local delivery.
 * Repeating identical stored output is a no-op; replacement is refused. */
UmiStatus UmiProviderTaskRecordOutput(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision,uint64_t bytes,const unsigned char sha256[UMI_SHA256_BYTES],
    UmiProviderTaskReceipt *out_receipt);
/* Verify scope, connection revision, content fingerprint and enabled settings.
 * Confirmed SUBMITTED/RUNNING gives POLL; SUCCEEDED gives FETCH_OUTPUT.
 * Other phases refuse recovery. This plan is not permission or a single-worker
 * lease. The host must approve the adapter destination, acquire a sole recovery
 * owner and recheck persisted receipt revision before performing any effect.
 * A matching fingerprint does not prove unchanged credential contents. */
UmiStatus UmiProviderTaskPlanRecovery(const UmiProviderTaskReceipt *receipt,
    const char *application_id,const char *profile_id,
    const UmiProviderConnection *connection,uint64_t connection_revision,
    UmiProviderTaskRecoveryAction *out_action);
#ifdef __cplusplus
}
#endif
#endif
