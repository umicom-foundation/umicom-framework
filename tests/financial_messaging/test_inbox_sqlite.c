/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_inbox_sqlite.c
 *
 * PURPOSE:
 *   Exercise restart, read-only access and two-connection journal admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/financial_messaging/inbox_store.h"
#include <errno.h>
int main(int argc, char **argv) {
  CHECK(argc == 3);
  /* CTest supplies a distinct disposable database in the build directory.
   * No application/user database is opened by this regression executable. */
  if (remove(argv[2]) != 0)
    CHECK(errno == ENOENT);
  UmiDataServer *server = NULL;
  OK(umi_data_server_create_sqlite(argv[2], &server));
  UmiUfmpEnvelope e = Envelope();
  UmiUfmpEnvelopePolicy p = Policy();
  UmiUfmpInboxReceipt original, replay;
  OK(UmiUfmpInboxReceive(server, &e, &p, "\x01", 1U, &original));
  CHECK(original.match == UMI_UFMP_REPLAY_NO_MATCH);
  CHECK(original.backend == UMI_DATA_BACKEND_SQLITE);
  if (strcmp(argv[1], "two-connections") == 0) {
    UmiDataServer *other = NULL;
    OK(umi_data_server_create_sqlite(argv[2], &other));
    OK(umi_data_server_begin(server));
    CHECK(UmiUfmpInboxReceive(other, &e, &p, "\x01", 1U, &replay) == UMI_STATUS_BUSY);
    OK(umi_data_server_rollback(server));
    OK(UmiUfmpInboxReceive(other, &e, &p, "\x01", 1U, &replay));
    CHECK(replay.match == UMI_UFMP_REPLAY_SAME_MESSAGE);
    umi_data_server_destroy(other);
  } else {
    umi_data_server_destroy(server);
    server = NULL;
    if (strcmp(argv[1], "readonly") == 0) {
      OK(umi_data_server_open_sqlite_readonly(argv[2], &server));
      UmiUfmpInboxRecord record;
      OK(UmiUfmpInboxLoad(server, original.receipt_id, &record));
      strcpy(e.message_id, "readonly.new");
      strcpy(e.idempotency_key, "readonly.operation");
      CHECK(UmiUfmpInboxReceive(server, &e, &p, "\x01", 1U, &replay) ==
            UMI_STATUS_PERMISSION_DENIED);
    } else {
      CHECK(strcmp(argv[1], "reopen") == 0);
      OK(umi_data_server_create_sqlite(argv[2], &server));
      strcpy(e.message_id, "after.restart");
      OK(UmiUfmpInboxReceive(server, &e, &p, "\x01", 1U, &replay));
      CHECK(replay.match == UMI_UFMP_REPLAY_SAME_OPERATION);
      CHECK(strcmp(replay.receipt_id, original.receipt_id) == 0);
      CHECK(umi_data_server_count(server) == 4U);
    }
  }
  umi_data_server_destroy(server);
  CHECK(remove(argv[2]) == 0);
  return 0;
}
