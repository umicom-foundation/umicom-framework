/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/outbox_example.c
 *
 * PURPOSE:
 *   Explain local recovery using synthetic messages and a disposable memory store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/outbox.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <string.h>
UmiStatus UmiUfmpOutboxExampleFormat(char *out, size_t capacity, size_t *required) {
  if (required == NULL || (out == NULL && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpEnvelope e = {0};
  e.revision = UMI_UFMP_LOGICAL_REVISION; e.type = UMI_UFMP_PAYMENT_INITIATE;
  e.format = UMI_UFMP_PAYLOAD_CONTROL_CBOR;
  strcpy(e.message_id, "synthetic.outgoing.1"); strcpy(e.conversation_id, "synthetic.flow");
  strcpy(e.idempotency_key, "synthetic.operation.1"); strcpy(e.sender, "synthetic.sender");
  strcpy(e.recipient, "synthetic.receiver"); strcpy(e.schema, "synthetic.profile");
  e.created_ms = 1000U; e.expires_ms = 5000U; e.payload_bytes = 1U;
  const unsigned char body = 1U;
  UmiSha256 sha; UmiSha256Init(&sha);
  UmiStatus s = UmiSha256Update(&sha, &body, 1U);
  if (s == UMI_STATUS_OK) s = UmiSha256Final(&sha, e.content_digest);
  UmiDataServer *server = NULL;
  if (s == UMI_STATUS_OK) s = umi_data_server_create_memory(&server);
  UmiUfmpEnvelopePolicy policy = {1100U, 0U, 5000U, 1024U, UMI_UFMP_LOGICAL_REVISION};
  UmiUfmpRetryPolicy retry = {3U, 100U, 400U};
  UmiUfmpOutboxRecord queued, attempt, waiting, retrying, review;
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxEnqueue(server, &e, &policy, &retry, &body, 1U, &queued);
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxTransition(server, queued.id, queued.revision,
    1100U, UMI_UFMP_OUTBOX_BEGIN, &attempt);
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxTransition(server, attempt.id, attempt.revision,
    1110U, UMI_UFMP_OUTBOX_NOT_SENT, &waiting);
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxTransition(server, waiting.id, waiting.revision,
    1210U, UMI_UFMP_OUTBOX_BEGIN, &retrying);
  if (s == UMI_STATUS_OK) s = UmiUfmpOutboxTransition(server, retrying.id, retrying.revision,
    1220U, UMI_UFMP_OUTBOX_UNCERTAIN, &review);
  umi_data_server_destroy(server);
  if (s != UMI_STATUS_OK) return s;
  char text[768];
  int n = snprintf(text, sizeof text,
    "Synthetic outgoing-message recovery (memory only)\n"
    "Queued before delivery: %s\n"
    "Known not sent: retry due at %llu ms\n"
    "Ambiguous outcome: %s\n"
    "Attempts: %u\nAutomatic retry after ambiguity: no\n"
    "Network requests: 0\nFinancial postings: 0\n",
    UmiUfmpOutboxStateName(queued.state), (unsigned long long)waiting.next_attempt_ms,
    UmiUfmpOutboxStateName(review.state), (unsigned)review.attempts);
  if (n < 0 || (size_t)n >= sizeof text) return UMI_STATUS_INTERNAL_ERROR;
  *required = (size_t)n + 1U;
  if (capacity < *required) return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, text, *required);
  return UMI_STATUS_OK;
}
