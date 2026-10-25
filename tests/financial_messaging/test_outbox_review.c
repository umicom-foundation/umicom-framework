/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_outbox_review.c
 *
 * PURPOSE:
 *   Check bounded record pages, stable exclusive cursors and partial journal damage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/financial_messaging/outbox_review.h"
static int Compare(const void *a, const void *b) { return strcmp(a, b); }
int main(int argc, char **argv) {
  const char *cases[] = {"empty", "pages", "invalid-limit", "invalid-cursor", "nested",
    "foreign-keys", "damaged-key", "orphan-state", "missing-state", "current-revision"};
  (void)Case(argc, argv, cases, sizeof cases / sizeof cases[0]);
  UmiDataServer *s = NULL; OK(umi_data_server_create_memory(&s));
  UmiUfmpOutboxPage page, sentinel; memset(&sentinel, 0x5a, sizeof sentinel); page = sentinel;
  if (strcmp(argv[1], "invalid-limit") == 0) {
    CHECK(UmiUfmpOutboxReadPage(s, NULL, 0U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUfmpOutboxReadPage(s, NULL, 33U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&page, &sentinel, sizeof page) == 0); goto done;
  }
  if (strcmp(argv[1], "invalid-cursor") == 0) {
    CHECK(UmiUfmpOutboxReadPage(s, "not-a-receipt", 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&page, &sentinel, sizeof page) == 0); goto done;
  }
  if (strcmp(argv[1], "nested") == 0) {
    OK(umi_data_server_begin(s));
    CHECK(UmiUfmpOutboxReadPage(s, NULL, 1U, &page) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(s)); OK(umi_data_server_rollback(s)); goto done;
  }
  if (strcmp(argv[1], "empty") == 0 || strcmp(argv[1], "foreign-keys") == 0) {
    if (strcmp(argv[1], "foreign-keys") == 0) OK(umi_data_server_set(s, "another.service/record", "private"));
    OK(UmiUfmpOutboxReadPage(s, NULL, 32U, &page));
    CHECK(page.count == 0U && !page.has_more && page.next_cursor[0] == '\0'); goto done;
  }
  if (strcmp(argv[1], "damaged-key") == 0) {
    OK(umi_data_server_set(s, "ufmp.outbox.v1/body/invalid", "private"));
    CHECK(UmiUfmpOutboxReadPage(s, NULL, 32U, &page) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&page, &sentinel, sizeof page) == 0); goto done;
  }
  UmiUfmpEnvelope e = Envelope(); UmiUfmpEnvelopePolicy p = Policy();
  UmiUfmpRetryPolicy retry = {3U, 10U, 40U}; UmiUfmpOutboxRecord r;
  if (strcmp(argv[1], "pages") == 0) {
    char ids[37][65];
    for (size_t i = 0U; i < 37U; ++i) {
      snprintf(e.message_id, sizeof e.message_id, "message.%zu", i);
      snprintf(e.idempotency_key, sizeof e.idempotency_key, "operation.%zu", i);
      OK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &r));
      memcpy(ids[i], r.id, sizeof ids[i]);
    }
    qsort(ids, 37U, sizeof ids[0], Compare);
    OK(UmiUfmpOutboxReadPage(s, NULL, 32U, &page));
    CHECK(page.count == 32U && page.has_more);
    for (size_t i = 0U; i < 32U; ++i) CHECK(strcmp(page.items[i].id, ids[i]) == 0);
    char cursor[65]; memcpy(cursor, page.next_cursor, sizeof cursor);
    OK(UmiUfmpOutboxReadPage(s, cursor, 32U, &page));
    CHECK(page.count == 5U && !page.has_more);
    for (size_t i = 0U; i < 5U; ++i) CHECK(strcmp(page.items[i].id, ids[i+32U]) == 0);
    memcpy(cursor, page.next_cursor, sizeof cursor);
    OK(UmiUfmpOutboxReadPage(s, cursor, 32U, &page)); CHECK(page.count == 0U);
  } else {
    OK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &r));
    if (strcmp(argv[1], "current-revision") == 0) {
      UmiUfmpOutboxRecord next;
      OK(UmiUfmpOutboxTransition(s, r.id, r.revision, 1100U, UMI_UFMP_OUTBOX_BEGIN, &next));
      OK(UmiUfmpOutboxReadPage(s, NULL, 1U, &page));
      CHECK(page.count == 1U && page.items[0].revision == next.revision);
      CHECK(page.items[0].state == UMI_UFMP_OUTBOX_IN_FLIGHT && page.items[0].attempts == 1U);
    } else {
      char key[192];
      snprintf(key, sizeof key, "ufmp.outbox.v1/%s/%s",
        strcmp(argv[1], "orphan-state") == 0 ? "body" : "state", r.id);
      OK(umi_data_server_delete(s, key));
      CHECK(UmiUfmpOutboxReadPage(s, NULL, 32U, &page) == UMI_STATUS_PARSE_ERROR);
      CHECK(memcmp(&page, &sentinel, sizeof page) == 0);
    }
  }
done:
  umi_data_server_destroy(s); return 0;
}
