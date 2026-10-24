/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Snapshot creation must include committed WAL pages and preserve occupied files. */
#include "readonly_fixture.h"
#include "umicom/data/backup.h"
#include "umicom/data/sqlite.h"
int main(int argc, char **argv) {
  const char *cases[] = {"roundtrip",   "wal",          "existing", "same-source",    "nested",
                         "memory",      "readonly",     "relative", "missing-parent", "unhealthy",
                         "extra-table", "later-source", "twice",    "null-output",    "empty",
                         "contended",   "long-path",    "legacy",   "null-server"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  const char *name = argv[1];
  char file[1024], target[1024];
  UmiDataServer *source = NULL, *reader = NULL, *backup = NULL;
  UmiDataBackupReport report, sentinel;
  memset(&sentinel, 0x53, sizeof(sentinel));
  report = sentinel;
  CHECK(ro_path(argv[2], name, file, true));
  int n = snprintf(target, sizeof(target), "%s.backup", file);
  CHECK(n > 0 && (size_t)n < sizeof(target));
  if (strcmp(name, "null-server") == 0) {
    CHECK(umi_data_backup_create_checked(NULL, target, &report) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (strcmp(name, "memory") == 0) {
    OK(umi_data_server_create_memory(&source));
    CHECK(umi_data_backup_create_checked(source, target, &report) == UMI_STATUS_UNAVAILABLE);
    goto refused;
  }
  OK(umi_data_server_create_sqlite(file, &source));
  OK(umi_data_server_set(source, "original", "first"));
  if (strcmp(name, "null-output") == 0) {
    CHECK(umi_data_backup_create_checked(source, target, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto refused;
  }
  if (strcmp(name, "empty") == 0) {
    CHECK(umi_data_backup_create_checked(source, "", &report) == UMI_STATUS_INVALID_ARGUMENT);
    goto refused;
  }
  if (strcmp(name, "relative") == 0) {
    CHECK(umi_data_backup_create_checked(source, "relative.sqlite", &report) ==
          UMI_STATUS_INVALID_ARGUMENT);
    goto refused;
  }
  if (strcmp(name, "long-path") == 0) {
    char large[1025];
    memset(large, 'x', sizeof(large));
    large[1024] = 0;
    CHECK(umi_data_backup_create_checked(source, large, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    goto refused;
  }
  if (strcmp(name, "same-source") == 0) {
    CHECK(umi_data_backup_create_checked(source, file, &report) == UMI_STATUS_ALREADY_EXISTS);
    goto refused;
  }
  if (strcmp(name, "existing") == 0) {
    FILE *occupied = fopen(target, "wbx");
    CHECK(occupied);
    size_t written = fwrite("retained", 1, 8, occupied);
    int closed = fclose(occupied);
    CHECK(written == 8 && closed == 0);
    CHECK(umi_data_backup_create_checked(source, target, &report) == UMI_STATUS_ALREADY_EXISTS);
    occupied = fopen(target, "rb");
    CHECK(occupied);
    char kept[9] = {0};
    size_t read = fread(kept, 1, 8, occupied);
    closed = fclose(occupied);
    CHECK(read == 8 && closed == 0 && strcmp(kept, "retained") == 0);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "nested") == 0) {
    OK(umi_data_server_begin(source));
    OK(umi_data_server_set(source, "pending", "kept transaction"));
    CHECK(umi_data_backup_create_checked(source, target, &report) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(source));
    OK(umi_data_server_rollback(source));
    goto refused;
  }
  if (strcmp(name, "unhealthy") == 0) {
    OK(umi_data_server_execute(
        source, "CREATE TABLE damaged(value CHECK(value>0)); PRAGMA ignore_check_constraints=ON; "
                "INSERT INTO damaged VALUES(-1); PRAGMA ignore_check_constraints=OFF;"));
    CHECK(umi_data_backup_create_checked(source, target, &report) == UMI_STATUS_INVALID_STATE);
    goto refused;
  }
  if (strcmp(name, "missing-parent") == 0) {
    n = snprintf(target, sizeof(target), "%s.absent/backup.sqlite", file);
    CHECK(n > 0 && (size_t)n < sizeof(target));
    CHECK(umi_data_backup_create_checked(source, target, &report) != UMI_STATUS_OK);
    goto refused;
  }
  if (strcmp(name, "contended") == 0) {
    OK(umi_data_server_execute(source, "PRAGMA busy_timeout=0;"));
    OK(umi_data_server_create_sqlite(file, &reader));
    OK(umi_data_server_execute(reader, "BEGIN EXCLUSIVE;"));
    CHECK(umi_data_backup_create_checked(source, target, &report) == UMI_STATUS_BUSY);
    OK(umi_data_server_rollback(reader));
    goto refused;
  }
  if (strcmp(name, "extra-table") == 0)
    OK(umi_data_server_execute(
        source, "CREATE TABLE extras(id INTEGER PRIMARY KEY); INSERT INTO extras VALUES(42);"));
  if (strcmp(name, "wal") == 0) {
    OK(umi_data_server_execute(
        source, "PRAGMA journal_mode=WAL; PRAGMA wal_autocheckpoint=0; PRAGMA busy_timeout=0;"));
    OK(umi_data_server_open_sqlite_readonly(file, &reader));
    OK(umi_data_server_begin(reader));
    char value[32];
    OK(umi_data_server_get(reader, "original", value, sizeof(value)));
    OK(umi_data_server_set(source, "original", "committed WAL"));
    CHECK(umi_sqlite_checkpoint(source) == UMI_STATUS_BUSY);
  }
  UmiDataServer *selected = source;
  if (strcmp(name, "readonly") == 0) {
    OK(umi_data_server_open_sqlite_readonly(file, &reader));
    selected = reader;
  }
  if (strcmp(name, "legacy") == 0)
    OK(umi_data_backup_create(selected, target));
  else {
    OK(umi_data_backup_create_checked(selected, target, &report));
    CHECK(report.page_count > 0 && report.record_count == 1 && report.sqlite_quick_check_passed &&
          strcmp(report.destination, target) == 0);
  }
  if (reader && umi_data_server_in_transaction(reader))
    OK(umi_data_server_rollback(reader));
  if (strcmp(name, "later-source") == 0 || strcmp(name, "twice") == 0)
    OK(umi_data_server_set(source, "original", "after snapshot"));
  if (strcmp(name, "twice") == 0) {
    report = sentinel;
    CHECK(umi_data_backup_create_checked(source, target, &report) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
  }
  OK(umi_data_server_open_sqlite_readonly(target, &backup));
  char value[32];
  OK(umi_data_server_get(backup, "original", value, sizeof(value)));
  CHECK(strcmp(value, strcmp(name, "wal") == 0 ? "committed WAL" : "first") == 0);
  umi_data_server_destroy(backup);
  backup = NULL;
  if (strcmp(name, "extra-table") == 0) {
    OK(umi_data_server_create_sqlite(target, &backup));
    OK(umi_data_server_execute(backup, "INSERT INTO extras VALUES(43);"));
    CHECK(umi_data_server_execute(backup, "INSERT INTO extras VALUES(42);") == UMI_STATUS_IO_ERROR);
  }
  goto cleanup;
refused:
  CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
  {
    FILE *probe = fopen(target, "rb");
    bool absent = probe == NULL;
    if (probe)
      fclose(probe);
    CHECK(absent);
  }
cleanup:
  if (reader && umi_data_server_in_transaction(reader))
    (void)umi_data_server_rollback(reader);
  if (source && umi_data_server_in_transaction(source))
    (void)umi_data_server_rollback(source);
  umi_data_server_destroy(backup);
  umi_data_server_destroy(reader);
  umi_data_server_destroy(source);
  return result;
}
