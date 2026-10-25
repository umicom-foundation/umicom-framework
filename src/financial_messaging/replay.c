/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/replay.c
 *
 * PURPOSE:
 *   Compare financial message and operation identities without claiming persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/replay.h"
#include "internal.h"
static int RecordValid(const UmiUfmpReplayRecord *record) {
  return record != NULL && record->revision == 1U &&
         UmiUfmpIdentifierValid(record->sender, sizeof record->sender) &&
         UmiUfmpIdentifierValid(record->message_id, sizeof record->message_id) &&
         (record->idempotency_key[0] == '\0' ||
          UmiUfmpIdentifierValid(record->idempotency_key, sizeof record->idempotency_key));
}
UmiStatus UmiUfmpReplayCapture(const UmiUfmpEnvelope *value, UmiUfmpReplayRecord *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiUfmpEnvelopeSyntax(value);
  if (status != UMI_STATUS_OK)
    return status;
  UmiUfmpReplayRecord record = {0};
  record.revision = 1U;
  memcpy(record.sender, value->sender, strlen(value->sender) + 1U);
  memcpy(record.message_id, value->message_id, strlen(value->message_id) + 1U);
  memcpy(record.idempotency_key, value->idempotency_key, strlen(value->idempotency_key) + 1U);
  status = UmiUfmpEnvelopeDigest(value, record.envelope_digest);
  if (status == UMI_STATUS_OK && value->idempotency_key[0] != '\0')
    status = UmiUfmpOperationDigest(value, record.operation_digest);
  if (status == UMI_STATUS_OK)
    *out = record;
  return status;
}
UmiStatus UmiUfmpReplayCompare(const UmiUfmpReplayRecord *stored, const UmiUfmpEnvelope *candidate,
                               UmiUfmpReplayMatch *out) {
  if (out == NULL || !RecordValid(stored))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpReplayRecord next;
  UmiStatus status = UmiUfmpReplayCapture(candidate, &next);
  if (status != UMI_STATUS_OK)
    return status;
  UmiUfmpReplayMatch match = UMI_UFMP_REPLAY_NO_MATCH;
  if (strcmp(stored->sender, next.sender) == 0) {
    if (strcmp(stored->message_id, next.message_id) == 0)
      match =
          memcmp(stored->envelope_digest, next.envelope_digest, sizeof next.envelope_digest) == 0
              ? UMI_UFMP_REPLAY_SAME_MESSAGE
              : UMI_UFMP_REPLAY_CONFLICT;
    else if (stored->idempotency_key[0] != '\0' &&
             strcmp(stored->idempotency_key, next.idempotency_key) == 0)
      match =
          memcmp(stored->operation_digest, next.operation_digest, sizeof next.operation_digest) == 0
              ? UMI_UFMP_REPLAY_SAME_OPERATION
              : UMI_UFMP_REPLAY_CONFLICT;
  }
  *out = match;
  return UMI_STATUS_OK;
}
