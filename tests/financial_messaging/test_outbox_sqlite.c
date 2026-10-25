/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_outbox_sqlite.c
 *
 * PURPOSE:
 *   Check outgoing recovery after reopen and revision arbitration across SQLite connections.
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
#include "umicom/financial_messaging/outbox_review.h"
#include <errno.h>
int main(int argc, char **argv) {
  CHECK(argc == 3);
  if (remove(argv[2]) != 0) CHECK(errno == ENOENT);
  UmiDataServer *s = NULL, *other = NULL;
  OK(umi_data_server_create_sqlite(argv[2], &s));
  UmiUfmpEnvelope e = Envelope(); UmiUfmpEnvelopePolicy p = Policy();
  UmiUfmpRetryPolicy retry = {3U, 10U, 40U};
  UmiUfmpOutboxRecord queued, flight, out;
  if (strcmp(argv[1], "rollback") == 0) {
    UmiUfmpOutboxRecord sentinel; memset(&sentinel, 0x5a, sizeof sentinel); out = sentinel;
    OK(umi_data_server_execute(s, "CREATE TRIGGER fail_outbox BEFORE INSERT ON umicom_kv "
      "WHEN NEW.key LIKE 'ufmp.outbox.v1/state/%' BEGIN SELECT RAISE(ABORT, 'synthetic state refusal'); END;"));
    CHECK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &out) == UMI_STATUS_IO_ERROR);
    CHECK(memcmp(&out, &sentinel, sizeof out) == 0);
    CHECK(umi_data_server_count(s) == 0U && !umi_data_server_in_transaction(s));
    OK(umi_data_server_execute(s, "DROP TRIGGER fail_outbox;"));
    OK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &out));
    CHECK(out.revision == 1U && umi_data_server_count(s) == 4U);
    umi_data_server_destroy(s); CHECK(remove(argv[2]) == 0); return 0;
  }
  OK(UmiUfmpOutboxEnqueue(s, &e, &p, &retry, "\x01", 1U, &queued));
  OK(UmiUfmpOutboxTransition(s, queued.id, queued.revision, 1100U, UMI_UFMP_OUTBOX_BEGIN, &flight));
  if (strcmp(argv[1], "competing") == 0) {
    OK(umi_data_server_create_sqlite(argv[2], &other));
    CHECK(UmiUfmpOutboxTransition(other, queued.id, queued.revision, 1100U,
      UMI_UFMP_OUTBOX_BEGIN, &out) == UMI_STATUS_BUSY);
    OK(umi_data_server_begin(s));
    CHECK(UmiUfmpOutboxTransition(other, flight.id, flight.revision, 1101U,
      UMI_UFMP_OUTBOX_RECEIPT, &out) == UMI_STATUS_BUSY);
    OK(umi_data_server_rollback(s));
    OK(UmiUfmpOutboxTransition(other, flight.id, flight.revision, 1101U,
      UMI_UFMP_OUTBOX_RECEIPT, &out));
    CHECK(out.state == UMI_UFMP_OUTBOX_RECEIVED);
  } else {
    umi_data_server_destroy(s); s = NULL;
    if (strcmp(argv[1], "readonly") == 0) {
      OK(umi_data_server_open_sqlite_readonly(argv[2], &s));
      OK(UmiUfmpOutboxLoad(s, flight.id, &out));
      CHECK(out.state == UMI_UFMP_OUTBOX_IN_FLIGHT);
      char report[16384], tiny[2] = {'x', '\0'}; size_t required = 0U;
      OK(UmiUfmpOutboxInspect(argv[2], NULL, report, sizeof report, &required));
      CHECK(strstr(report, "in-flight") != NULL && strstr(report, "Records: 1") != NULL);
      CHECK(UmiUfmpOutboxInspect(argv[2], NULL, tiny, sizeof tiny, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
      CHECK(tiny[0] == 'x' && tiny[1] == '\0' && required == strlen(report) + 1U);
      CHECK(umi_data_server_count(s) == 4U);
      CHECK(UmiUfmpOutboxTransition(s, flight.id, flight.revision, 1101U,
        UMI_UFMP_OUTBOX_RECOVER, &out) == UMI_STATUS_PERMISSION_DENIED);
    } else {
      CHECK(strcmp(argv[1], "reopen") == 0);
      OK(umi_data_server_create_sqlite(argv[2], &s));
      OK(UmiUfmpOutboxLoad(s, flight.id, &out));
      CHECK(out.state == UMI_UFMP_OUTBOX_IN_FLIGHT && out.attempts == 1U);
      OK(UmiUfmpOutboxTransition(s, flight.id, flight.revision, 1101U,
        UMI_UFMP_OUTBOX_RECOVER, &out));
      CHECK(out.state == UMI_UFMP_OUTBOX_REVIEW);
      CHECK(UmiUfmpOutboxTransition(s, out.id, out.revision, 1102U,
        UMI_UFMP_OUTBOX_BEGIN, &queued) == UMI_STATUS_INVALID_STATE);
      umi_data_server_destroy(s); s = NULL;
      OK(umi_data_server_create_sqlite(argv[2], &s));
      OK(UmiUfmpOutboxLoad(s, flight.id, &out));
      CHECK(out.state == UMI_UFMP_OUTBOX_REVIEW && out.message.payload[0] == 1U);
    }
  }
  umi_data_server_destroy(other); umi_data_server_destroy(s);
  CHECK(remove(argv[2]) == 0);
  return 0;
}
