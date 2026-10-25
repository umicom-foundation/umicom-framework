/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/replay.h
 *
 * PURPOSE:
 *   Separate repeated delivery from conflicting reuse of a financial operation key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_REPLAY_H
#define UMICOM_FINANCIAL_MESSAGING_REPLAY_H
#include "umicom/financial_messaging/envelope.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUfmpReplayRecord {
  uint32_t revision;
  char sender[UMI_UFMP_ID_CAPACITY];
  char message_id[UMI_UFMP_ID_CAPACITY];
  char idempotency_key[UMI_UFMP_ID_CAPACITY];
  unsigned char envelope_digest[UMI_UFMP_DIGEST_BYTES];
  unsigned char operation_digest[UMI_UFMP_DIGEST_BYTES];
} UmiUfmpReplayRecord;
typedef enum UmiUfmpReplayMatch {
  UMI_UFMP_REPLAY_NO_MATCH = 0,
  UMI_UFMP_REPLAY_SAME_MESSAGE = 1,
  UMI_UFMP_REPLAY_SAME_OPERATION = 2,
  UMI_UFMP_REPLAY_CONFLICT = 3
} UmiUfmpReplayMatch;
/* Capture a CANDIDATE record only. The Data Server transaction must atomically
 * enforce uniqueness and persist this record with the business effect/result.
 * Never send a durable acknowledgement based on this in-memory value.
 * Check authentication, schema, freshness and payload integrity first.
 * Output remains unchanged on error; source and output must not overlap. */
UmiStatus UmiUfmpReplayCapture(const UmiUfmpEnvelope *value, UmiUfmpReplayRecord *out);
/* Compare a trusted stored record with a syntactically valid candidate.
 * Scope is the exact sender ID. Same sender/key with changed destination,
 * schema, type or payload is a conflict even when the delivery ID changes.
 * Same message ID with any changed envelope field is also a conflict.
 * A match is not permission to execute again: return the durable prior result.
 * The caller serializes lookup/insert under Data Server ownership; this pure
 * comparison cannot provide concurrency control or crash recovery by itself. */
UmiStatus UmiUfmpReplayCompare(const UmiUfmpReplayRecord *stored, const UmiUfmpEnvelope *candidate,
                               UmiUfmpReplayMatch *out);
#ifdef __cplusplus
}
#endif
#endif
