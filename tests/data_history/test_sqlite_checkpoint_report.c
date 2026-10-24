/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A blocked FULL checkpoint is BUSY even when its SQL wrapper could execute. */
#include "readonly_fixture.h"
#include "umicom/data/sqlite.h"
int main(int argc, char **argv) {
  const char *cases[] = {"wal",    "non-wal", "busy", "legacy-busy", "readonly",
                         "nested", "memory",  "null", "null-output", "retry"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  const char *name = argv[1];
  char file[1024];
  UmiDataServer *writer = NULL, *reader = NULL;
  UmiSqliteCheckpointReport report, sentinel;
  memset(&sentinel, 0x6c, sizeof(sentinel));
  report = sentinel;
  if (strcmp(name, "null") == 0) {
    CHECK(umi_sqlite_checkpoint_full(NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "memory") == 0) {
    OK(umi_data_server_create_memory(&writer));
    CHECK(umi_sqlite_checkpoint_full(writer, &report) == UMI_STATUS_UNAVAILABLE);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    goto cleanup;
  }
  CHECK(ro_path(argv[2], name, file, true));
  OK(umi_data_server_create_sqlite(file, &writer));
  if (strcmp(name, "null-output") == 0) {
    CHECK(umi_sqlite_checkpoint_full(writer, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (strcmp(name, "non-wal") == 0) {
    OK(umi_sqlite_checkpoint_full(writer, &report));
    CHECK(!report.wal_mode && report.log_frames == -1 && report.checkpointed_frames == -1);
    goto cleanup;
  }
  OK(umi_data_server_execute(
      writer, "PRAGMA journal_mode=WAL; PRAGMA wal_autocheckpoint=0; PRAGMA busy_timeout=0;"));
  OK(umi_data_server_set(writer, "original", "first"));
  if (strcmp(name, "nested") == 0) {
    OK(umi_data_server_begin(writer));
    CHECK(umi_sqlite_checkpoint_full(writer, &report) == UMI_STATUS_BUSY);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    CHECK(umi_data_server_in_transaction(writer));
    OK(umi_data_server_rollback(writer));
    goto cleanup;
  }
  if (strcmp(name, "readonly") == 0) {
    OK(umi_data_server_open_sqlite_readonly(file, &reader));
    CHECK(umi_sqlite_checkpoint_full(reader, &report) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    goto cleanup;
  }
  bool blocked =
      strcmp(name, "busy") == 0 || strcmp(name, "legacy-busy") == 0 || strcmp(name, "retry") == 0;
  if (blocked) {
    OK(umi_data_server_open_sqlite_readonly(file, &reader));
    OK(umi_data_server_begin(reader));
    char value[32];
    OK(umi_data_server_get(reader, "original", value, sizeof(value)));
    OK(umi_data_server_set(writer, "original", "later"));
    CHECK(umi_sqlite_checkpoint_full(writer, &report) == UMI_STATUS_BUSY);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    CHECK(umi_sqlite_checkpoint(writer) == UMI_STATUS_BUSY);
    OK(umi_data_server_rollback(reader));
  }
  OK(umi_sqlite_checkpoint_full(writer, &report));
  CHECK(report.wal_mode && report.log_frames >= 0 &&
        report.checkpointed_frames == report.log_frames);
  OK(umi_sqlite_checkpoint(writer));
cleanup:
  if (reader && umi_data_server_in_transaction(reader))
    (void)umi_data_server_rollback(reader);
  if (writer && umi_data_server_in_transaction(writer))
    (void)umi_data_server_rollback(writer);
  umi_data_server_destroy(reader);
  umi_data_server_destroy(writer);
  return result;
}
