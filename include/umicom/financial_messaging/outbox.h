/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/outbox.h
 *
 * PURPOSE:
 *   Own outgoing delivery state independently from payment or settlement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_OUTBOX_H
#define UMICOM_FINANCIAL_MESSAGING_OUTBOX_H
#include "umicom/financial_messaging/inbox_store.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiUfmpOutboxState {
  UMI_UFMP_OUTBOX_READY = 1, UMI_UFMP_OUTBOX_IN_FLIGHT,
  UMI_UFMP_OUTBOX_RECEIVED, UMI_UFMP_OUTBOX_REVIEW, UMI_UFMP_OUTBOX_EXPIRED
} UmiUfmpOutboxState;
typedef enum UmiUfmpOutboxEvent {
  UMI_UFMP_OUTBOX_BEGIN = 1, UMI_UFMP_OUTBOX_RECEIPT, UMI_UFMP_OUTBOX_NOT_SENT,
  UMI_UFMP_OUTBOX_UNCERTAIN, UMI_UFMP_OUTBOX_RECOVER
} UmiUfmpOutboxEvent;
typedef struct UmiUfmpRetryPolicy {
  uint32_t maximum_attempts; /* 1..32 including the first attempt. */
  uint64_t initial_delay_ms; /* Positive; capped exponential retry delay. */
  uint64_t maximum_delay_ms;
} UmiUfmpRetryPolicy;
typedef struct UmiUfmpOutboxRecord {
  char id[UMI_UFMP_RECEIPT_CAPACITY];
  UmiUfmpInboxRecord message;
  UmiUfmpRetryPolicy retry;
  uint64_t revision, updated_ms, next_attempt_ms;
  uint32_t attempts;
  UmiUfmpOutboxState state;
} UmiUfmpOutboxRecord;
/* Local outgoing intent, not a transport or financial posting. Only enqueue
 * authenticated, authorized, schema-checked outgoing messages. Body and retry
 * policy are immutable. An exact duplicate returns its existing record.
 * Reusing a sender/message or sender/operation identity with another envelope
 * or policy returns ALREADY_EXISTS, including after expiry.
 *
 * One database is one outbox scope. Keys under ufmp.outbox.v1/ belong exclusively
 * here. SQLite durability depends on configured storage; memory is disposable.
 * No call sends bytes or updates a ledger. Payload limit matches the inbox.
 * Mutations own begin/commit/rollback; nested use returns BUSY. Outputs remain
 * unchanged on failure. Do not overlap input/output storage.
 */
UmiStatus UmiUfmpOutboxEnqueue(UmiDataServer *server, const UmiUfmpEnvelope *envelope,
  const UmiUfmpEnvelopePolicy *policy, const UmiUfmpRetryPolicy *retry,
  const void *payload, size_t length, UmiUfmpOutboxRecord *out);
UmiStatus UmiUfmpOutboxLoad(const UmiDataServer *server, const char *id,
  UmiUfmpOutboxRecord *out);
/* expected_revision is the last observed revision, not a lock across I/O.
 * BEGIN commits IN_FLIGHT before the caller may send. A stale revision returns
 * BUSY, including late callbacks from an earlier attempt. Send outside the
 * database transaction, and never send after a failed BEGIN.
 *
 * RECEIPT requires independently authenticated transport-receipt evidence.
 * It records delivery only, never payment acceptance, settlement or credit.
 * NOT_SENT requires proof that NO bytes reached a transport. Only this outcome
 * schedules capped backoff, subject to attempt limits and envelope expiry.
 * UNCERTAIN quarantines ambiguity for reconciliation.
 * RECOVER quarantines IN_FLIGHT only after the old worker has been stopped.
 * Never recover while that worker can still transmit. Neither ambiguity nor
 * restart automatically resends a payment. RECOVER also expires overdue READY
 * entries; other READY entries reject it. Terminal states have no retry route.
 *
 * now_ms is a caller-supplied trusted clock. Backward time is rejected.
 * BEGIN before the due time returns BUSY; at expiry it returns TIMEOUT without
 * mutation. Use RECOVER to persist EXPIRED. Arithmetic is overflow checked.
 */
UmiStatus UmiUfmpOutboxTransition(UmiDataServer *server, const char *id,
  uint64_t expected_revision, uint64_t now_ms, UmiUfmpOutboxEvent event,
  UmiUfmpOutboxRecord *out);
const char *UmiUfmpOutboxStateName(UmiUfmpOutboxState state);
/* Synthetic memory-backed walkthrough; no network or application database. */
UmiStatus UmiUfmpOutboxExampleFormat(char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
