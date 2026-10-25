/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/outbox_store.c
 *
 * PURPOSE:
 *   Commit outgoing intent and revision-checked delivery transitions atomically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "outbox_internal.h"
#include <stdio.h>
static UmiStatus Key(char out[UMI_UFMP_INBOX_KEY_CAPACITY], const char *kind,
                     const char *sender, const char *id) {
  int n = sender == NULL ? snprintf(out, UMI_UFMP_INBOX_KEY_CAPACITY,
    "ufmp.outbox.v1/%s/%s", kind, id) : snprintf(out, UMI_UFMP_INBOX_KEY_CAPACITY,
    "ufmp.outbox.v1/%s/%s/%s", kind, sender, id);
  return n < 0 || (size_t)n >= UMI_UFMP_INBOX_KEY_CAPACITY ?
    UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
static UmiStatus Identity(const UmiUfmpEnvelope *e, char id[65]) {
  unsigned char digest[32];
  UmiStatus s = UmiUfmpEnvelopeDigest(e, digest);
  if (s == UMI_STATUS_OK) UmiUfmpInboxHex(digest, sizeof digest, id);
  return s;
}
static UmiStatus Read(const UmiDataServer *server, const char *kind, const char *sender,
                      const char *id, char *out, size_t capacity) {
  char key[UMI_UFMP_INBOX_KEY_CAPACITY];
  UmiStatus s = Key(key, kind, sender, id);
  if (s == UMI_STATUS_OK) s = umi_data_server_get(server, key, out, capacity);
  return s == UMI_STATUS_CAPACITY_EXCEEDED ? UMI_STATUS_PARSE_ERROR : s;
}
static UmiStatus Write(UmiDataServer *server, const char *kind, const char *sender,
                       const char *id, const char *value) {
  char key[UMI_UFMP_INBOX_KEY_CAPACITY];
  UmiStatus s = Key(key, kind, sender, id);
  return s == UMI_STATUS_OK ? umi_data_server_set(server, key, value) : s;
}
UmiStatus UmiUfmpOutboxLoad(const UmiDataServer *server, const char *id,
                            UmiUfmpOutboxRecord *out) {
  if (server == NULL || out == NULL || !UmiUfmpInboxReceiptValid(id)) return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpOutboxRecord r = {0};
  char text[UMI_UFMP_INBOX_TEXT_CAPACITY] = {0}, identity[65], state[256] = {0};
  UmiStatus s = Read(server, "body", NULL, id, text, sizeof text);
  /* A surviving state without its immutable body is corruption, not an
   * unknown job that a caller may safely replace. Preserve the first lookup
   * path while distinguishing this partial-record case. */
#if 0
  if (s != UMI_STATUS_OK) return s;
#endif
  if (s == UMI_STATUS_NOT_FOUND) {
    UmiStatus state_status = Read(server, "state", NULL, id, state, sizeof state);
    return state_status == UMI_STATUS_OK ? UMI_STATUS_PARSE_ERROR : state_status;
  }
  if (s != UMI_STATUS_OK) return s;
  s = UmiUfmpInboxDecode(text, &r.message);
  if (s == UMI_STATUS_OK) s = Identity(&r.message.envelope, identity);
  if (s != UMI_STATUS_OK || strcmp(identity, id) != 0) return UMI_STATUS_PARSE_ERROR;
  s = Read(server, "state", NULL, id, state, sizeof state);
  if (s != UMI_STATUS_OK) return s == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : s;
  memcpy(r.id, id, sizeof r.id);
  s = UmiUfmpOutboxStateDecode(state, &r);
  if (s == UMI_STATUS_OK) *out = r;
  return s;
}
static int PolicyEqual(const UmiUfmpRetryPolicy *a, const UmiUfmpRetryPolicy *b) {
  return a->maximum_attempts == b->maximum_attempts &&
    a->initial_delay_ms == b->initial_delay_ms && a->maximum_delay_ms == b->maximum_delay_ms;
}
static UmiStatus Existing(const UmiDataServer *server, const char *alias,
                           const UmiUfmpOutboxRecord *candidate, UmiUfmpOutboxRecord *out) {
  if (!UmiUfmpInboxReceiptValid(alias)) return UMI_STATUS_PARSE_ERROR;
  UmiUfmpOutboxRecord stored;
  UmiStatus s = UmiUfmpOutboxLoad(server, alias, &stored);
  if (s != UMI_STATUS_OK) return s == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : s;
  /* Do not silently replace an expired envelope with a fresh one. A transport
   * retry uses this exact stored message, while a changed envelope needs a
   * separately reviewed business operation and identity. */
  if (strcmp(alias, candidate->id) != 0 || !PolicyEqual(&stored.retry, &candidate->retry))
    return UMI_STATUS_ALREADY_EXISTS;
  *out = stored;
  return UMI_STATUS_OK;
}
static UmiStatus EnqueueOwned(UmiDataServer *server, UmiUfmpOutboxRecord *r) {
  char message[65] = {0}, operation[65] = {0};
  const UmiUfmpEnvelope *e = &r->message.envelope;
  UmiStatus m = Read(server, "message", e->sender, e->message_id, message, sizeof message);
  UmiStatus o = e->idempotency_key[0] == '\0' ? UMI_STATUS_NOT_FOUND :
    Read(server, "operation", e->sender, e->idempotency_key, operation, sizeof operation);
  if (m != UMI_STATUS_OK && m != UMI_STATUS_NOT_FOUND) return m;
  if (o != UMI_STATUS_OK && o != UMI_STATUS_NOT_FOUND) return o;
  if (m == UMI_STATUS_OK) {
    /* Identify a reused message before examining the candidate operation
     * alias. A different operation is a caller conflict, not journal damage.
     * Keep the original admission order for review. */
#if 0
    if (e->idempotency_key[0] != '\0' &&
        (o != UMI_STATUS_OK || strcmp(message, operation) != 0)) return UMI_STATUS_PARSE_ERROR;
    UmiUfmpOutboxRecord stored;
    UmiStatus s = Existing(server, message, r, &stored);
#endif
    UmiUfmpOutboxRecord stored;
    UmiStatus s = Existing(server, message, r, &stored);
    if (s == UMI_STATUS_OK && e->idempotency_key[0] != '\0' &&
        (o != UMI_STATUS_OK || strcmp(message, operation) != 0)) return UMI_STATUS_PARSE_ERROR;
    if (s == UMI_STATUS_OK) *r = stored;
    return s;
  }
  if (o == UMI_STATUS_OK) {
    UmiUfmpOutboxRecord stored;
    UmiStatus s = Existing(server, operation, r, &stored);
    /* Matching original identity but a missing message index is damage. */
    return s == UMI_STATUS_OK ? UMI_STATUS_PARSE_ERROR : s;
  }
  char old[UMI_UFMP_INBOX_TEXT_CAPACITY];
  UmiStatus s = Read(server, "body", NULL, r->id, old, sizeof old);
  if (s == UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
  if (s != UMI_STATUS_NOT_FOUND) return s;
  s = Read(server, "state", NULL, r->id, old, sizeof old);
  if (s == UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
  if (s != UMI_STATUS_NOT_FOUND) return s;
  char body[UMI_UFMP_INBOX_TEXT_CAPACITY], state[256];
  s = UmiUfmpInboxEncode(&r->message, body);
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxStateEncode(r, state);
  if (s == UMI_STATUS_OK) s = Write(server, "body", NULL, r->id, body);
  if (s == UMI_STATUS_OK) s = Write(server, "state", NULL, r->id, state);
  if (s == UMI_STATUS_OK) s = Write(server, "message", e->sender, e->message_id, r->id);
  if (s == UMI_STATUS_OK && e->idempotency_key[0] != '\0')
    s = Write(server, "operation", e->sender, e->idempotency_key, r->id);
  return s;
}
static UmiStatus Begin(UmiDataServer *server) {
  return umi_data_server_in_transaction(server) ? UMI_STATUS_BUSY : umi_data_server_begin(server);
}
static UmiStatus Finish(UmiDataServer *server, UmiStatus status) {
  if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);
  if (status != UMI_STATUS_OK) {
    UmiStatus rollback = umi_data_server_rollback(server);
    if (rollback != UMI_STATUS_OK) return rollback;
  }
  return status;
}
UmiStatus UmiUfmpOutboxEnqueue(UmiDataServer *server, const UmiUfmpEnvelope *e,
  const UmiUfmpEnvelopePolicy *policy, const UmiUfmpRetryPolicy *retry,
  const void *payload, size_t length, UmiUfmpOutboxRecord *out) {
  if (server == NULL || out == NULL || !UmiUfmpRetryValid(retry)) return UMI_STATUS_INVALID_ARGUMENT;
  if (length > UMI_UFMP_INBOX_PAYLOAD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStatus s = UmiUfmpEnvelopeValidate(e, policy);
  if (s == UMI_STATUS_OK) s = UmiUfmpEnvelopeCheckPayload(e, payload, length);
  if (s != UMI_STATUS_OK) return s;
  UmiUfmpOutboxRecord r = {0};
  r.message.envelope = *e; r.message.payload_length = length;
  if (length) memcpy(r.message.payload, payload, length);
  r.retry = *retry; r.revision = 1U; r.updated_ms = policy->now_ms;
  r.next_attempt_ms = policy->now_ms > e->created_ms ? policy->now_ms : e->created_ms;
  r.state = UMI_UFMP_OUTBOX_READY;
  s = Identity(e, r.id);
  if (s != UMI_STATUS_OK) return s;
  s = Begin(server);
  if (s != UMI_STATUS_OK) return s;
  s = Finish(server, EnqueueOwned(server, &r));
  if (s == UMI_STATUS_OK) *out = r;
  return s;
}
UmiStatus UmiUfmpOutboxTransition(UmiDataServer *server, const char *id,
  uint64_t revision, uint64_t now, UmiUfmpOutboxEvent event, UmiUfmpOutboxRecord *out) {
  if (server == NULL || out == NULL || revision == 0U || !UmiUfmpInboxReceiptValid(id))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus s = Begin(server);
  if (s != UMI_STATUS_OK) return s;
  UmiUfmpOutboxRecord r;
  s = UmiUfmpOutboxLoad(server, id, &r);
  if (s == UMI_STATUS_OK && r.revision != revision) s = UMI_STATUS_BUSY;
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxApply(&r, now, event);
  char state[256];
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxStateEncode(&r, state);
  if (s == UMI_STATUS_OK) s = Write(server, "state", NULL, id, state);
  s = Finish(server, s);
  if (s == UMI_STATUS_OK) *out = r;
  return s;
}
