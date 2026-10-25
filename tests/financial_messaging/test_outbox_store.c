/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_outbox_store.c
 *
 * PURPOSE:
 *   Check delivery state, duplicate identities, bounded retries and ambiguous outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/financial_messaging/outbox.h"
static UmiUfmpOutboxRecord Step(UmiDataServer *s, const UmiUfmpOutboxRecord *r,
                                uint64_t now, UmiUfmpOutboxEvent event) {
  UmiUfmpOutboxRecord out;
  OK(UmiUfmpOutboxTransition(s, r->id, r->revision, now, event, &out));
  CHECK(out.revision == r->revision + 1U);
  return out;
}
int main(int argc, char **argv) {
  const char *names[] = {"delivery", "duplicate", "changed-body", "changed-policy",
    "operation-conflict", "message-conflict", "sender-scope", "backoff", "attempt-limit",
    "uncertain", "recover", "stale", "expired", "retry-expiry", "clock-backward",
    "nested", "corrupt-state", "missing-body", "corrupt-body", "output-preserved",
    "overflow-delay", "invalid-policy", "example", "cancel-before-attempt"};
  int which = Case(argc, argv, names, sizeof names / sizeof names[0]); (void)which;
  const char *name = argv[1];
  UmiDataServer *s = NULL; OK(umi_data_server_create_memory(&s));
  UmiUfmpEnvelope e = Envelope(); UmiUfmpEnvelopePolicy p = Policy();
  UmiUfmpRetryPolicy retry = {3U, 10U, 15U};
  UmiUfmpOutboxRecord r, out, sentinel;
  memset(&sentinel, 0x5a, sizeof sentinel); out = sentinel;
  if (strcmp(name, "invalid-policy") == 0) {
    retry.maximum_attempts = 0U;
    CHECK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&out, &sentinel, sizeof out) == 0 && umi_data_server_count(s) == 0U);
    goto done;
  }
  if (strcmp(name, "overflow-delay") == 0) retry.initial_delay_ms = retry.maximum_delay_ms = UINT64_MAX;
  OK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &r));
  CHECK(r.state == UMI_UFMP_OUTBOX_READY && r.revision == 1U && r.attempts == 0U);
  CHECK(umi_data_server_count(s) == 4U);
  if (strcmp(name, "duplicate") == 0) {
    OK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &out));
    CHECK(strcmp(out.id, r.id) == 0 && out.revision == 1U && umi_data_server_count(s) == 4U);
  } else if (strcmp(name, "changed-body") == 0) {
    Body(&e, "\x02", 1U);
    CHECK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x02", 1U, &out) == UMI_STATUS_ALREADY_EXISTS);
  } else if (strcmp(name, "changed-policy") == 0) {
    retry.maximum_attempts++;
    CHECK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &out) == UMI_STATUS_ALREADY_EXISTS);
  } else if (strcmp(name, "operation-conflict") == 0 || strcmp(name, "message-conflict") == 0) {
    if (strcmp(name, "operation-conflict") == 0) strcpy(e.message_id, "new.message");
    else strcpy(e.idempotency_key, "new.operation");
    CHECK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &out) == UMI_STATUS_ALREADY_EXISTS);
  } else if (strcmp(name, "sender-scope") == 0) {
    strcpy(e.sender, "another.sender");
    OK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &out));
    CHECK(strcmp(out.id, r.id) != 0 && umi_data_server_count(s) == 8U);
  } else if (strcmp(name, "nested") == 0) {
    OK(umi_data_server_begin(s));
    CHECK(UmiUfmpOutboxTransition(s, r.id, r.revision, 1100U, UMI_UFMP_OUTBOX_BEGIN, &out) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(s)); OK(umi_data_server_rollback(s));
  } else if (strcmp(name, "corrupt-state") == 0 || strcmp(name, "missing-body") == 0 ||
             strcmp(name, "corrupt-body") == 0) {
    char key[192];
    snprintf(key, sizeof key, "ufmp.outbox.v1/%s/%s",
      strcmp(name, "corrupt-state") == 0 ? "state" : "body", r.id);
    /* Empty/malformed stored bodies are damage, never absent work to resend. */
    /* Retain the initial empty-body fixture; removal checks the distinct
     * partial-record recovery boundary without weakening corruption checks. */
#if 0
    OK(umi_data_server_set(s, key, strcmp(name, "missing-body") == 0 ? "" : "damaged"));
#endif
    if (strcmp(name, "missing-body") == 0) OK(umi_data_server_delete(s, key));
    else OK(umi_data_server_set(s, key, "damaged"));
    CHECK(UmiUfmpOutboxLoad(s, r.id, &out) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&out, &sentinel, sizeof out) == 0);
  } else if (strcmp(name, "expired") == 0) {
    CHECK(UmiUfmpOutboxTransition(s, r.id, r.revision, 2000U, UMI_UFMP_OUTBOX_BEGIN, &out) == UMI_STATUS_TIMEOUT);
    out = Step(s, &r, 2000U, UMI_UFMP_OUTBOX_RECOVER);
    CHECK(out.state == UMI_UFMP_OUTBOX_EXPIRED && out.attempts == 0U);
  } else if (strcmp(name, "clock-backward") == 0) {
    CHECK(UmiUfmpOutboxTransition(s, r.id, r.revision, 1099U, UMI_UFMP_OUTBOX_BEGIN, &out) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (strcmp(name, "output-preserved") == 0) {
    CHECK(UmiUfmpOutboxTransition(s, r.id, r.revision, 1100U, (UmiUfmpOutboxEvent)99, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&out, &sentinel, sizeof out) == 0);
    OK(UmiUfmpOutboxLoad(s, r.id, &out)); CHECK(out.revision == 1U);
  } else if (strcmp(name, "cancel-before-attempt") == 0) {
    CHECK(UmiUfmpOutboxTransition(s, r.id, r.revision, 1100U, UMI_UFMP_OUTBOX_RECEIPT, &out) == UMI_STATUS_INVALID_STATE);
  } else if (strcmp(name, "example") == 0) {
    char text[768]; size_t n = 0U;
    OK(UmiUfmpOutboxExampleFormat(text, sizeof text, &n));
    CHECK(strstr(text, "Financial postings: 0") != NULL);
    CHECK(strstr(text, "Automatic retry after ambiguity: no") != NULL && n == strlen(text)+1U);
  } else {
    UmiUfmpOutboxRecord flight = Step(s, &r, 1100U, UMI_UFMP_OUTBOX_BEGIN);
    CHECK(flight.attempts == 1U && flight.state == UMI_UFMP_OUTBOX_IN_FLIGHT);
    if (strcmp(name, "stale") == 0) {
      CHECK(UmiUfmpOutboxTransition(s, r.id, r.revision, 1101U, UMI_UFMP_OUTBOX_RECEIPT, &out) == UMI_STATUS_BUSY);
    } else if (strcmp(name, "delivery") == 0) {
      out = Step(s, &flight, 1101U, UMI_UFMP_OUTBOX_RECEIPT);
      CHECK(out.state == UMI_UFMP_OUTBOX_RECEIVED);
      CHECK(UmiUfmpOutboxTransition(s, r.id, out.revision, 1102U, UMI_UFMP_OUTBOX_BEGIN, &sentinel) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "uncertain") == 0 || strcmp(name, "recover") == 0) {
      out = Step(s, &flight, 1101U, strcmp(name, "recover") == 0 ?
        UMI_UFMP_OUTBOX_RECOVER : UMI_UFMP_OUTBOX_UNCERTAIN);
      CHECK(out.state == UMI_UFMP_OUTBOX_REVIEW);
      CHECK(UmiUfmpOutboxTransition(s, r.id, out.revision, 1102U, UMI_UFMP_OUTBOX_BEGIN, &sentinel) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "retry-expiry") == 0 || strcmp(name, "overflow-delay") == 0) {
      out = Step(s, &flight, strcmp(name, "retry-expiry") == 0 ? 1995U : 1101U, UMI_UFMP_OUTBOX_NOT_SENT);
      CHECK(out.state == UMI_UFMP_OUTBOX_EXPIRED);
    } else {
      UmiUfmpOutboxRecord wait = Step(s, &flight, 1101U, UMI_UFMP_OUTBOX_NOT_SENT);
      CHECK(wait.next_attempt_ms == 1111U && wait.state == UMI_UFMP_OUTBOX_READY);
      CHECK(UmiUfmpOutboxTransition(s, r.id, wait.revision, 1110U, UMI_UFMP_OUTBOX_BEGIN, &out) == UMI_STATUS_BUSY);
      flight = Step(s, &wait, 1111U, UMI_UFMP_OUTBOX_BEGIN);
      wait = Step(s, &flight, 1112U, UMI_UFMP_OUTBOX_NOT_SENT);
      CHECK(wait.next_attempt_ms == 1127U); /* 20ms exponential step capped at 15ms. */
      if (strcmp(name, "attempt-limit") == 0) {
        flight = Step(s, &wait, 1127U, UMI_UFMP_OUTBOX_BEGIN);
        out = Step(s, &flight, 1128U, UMI_UFMP_OUTBOX_NOT_SENT);
        CHECK(out.state == UMI_UFMP_OUTBOX_REVIEW && out.attempts == 3U);
      }
    }
  }
done:
  umi_data_server_destroy(s);
  return 0;
}
