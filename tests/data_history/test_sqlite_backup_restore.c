/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_sqlite_backup_restore.c
 *
 * PURPOSE:
 *   Recovery creates a new store; existing destinations and source bytes survive.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Recovery creates a new store; existing destinations and source bytes survive. */
#include "readonly_fixture.h"
#include "umicom/data/backup.h"
#include "umicom/data/sqlite.h"
int main(int argc, char **argv) {
  const char *cases[] = {"roundtrip",   "existing",       "same-source",     "missing-source",
                         "malformed",   "unrelated",      "relative-source", "relative-target",
                         "unhealthy",   "legacy",         "source-wal",      "null-output",
                         "null-source", "source-retained"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  const char *name = argv[1];
  UmiDataServer *source = NULL, *restored = NULL;
  char file[1024], target[1024], selected[1024];
  UmiDataBackupReport report, sentinel;
  memset(&sentinel, 0x47, sizeof(sentinel));
  report = sentinel;
  CHECK(ro_path(argv[2], name, file, true));
  int n = snprintf(target, sizeof(target), "%s.recovered", file);
  CHECK(n > 0 && (size_t)n < sizeof(target));
  memcpy(selected, file, sizeof(selected));
  if (strcmp(name, "null-source") == 0) {
    CHECK(umi_data_backup_restore_checked(NULL, target, &report) == UMI_STATUS_INVALID_ARGUMENT);
    goto refused;
  }
  if (strcmp(name, "malformed") == 0) {
    FILE *bad = fopen(file, "wb");
    CHECK(bad);
    size_t written = fwrite("broken SQLite", 1, 13, bad);
    int closed = fclose(bad);
    CHECK(written == 13 && closed == 0);
    CHECK(umi_data_backup_restore_checked(file, target, &report) != UMI_STATUS_OK);
    goto refused;
  }
  OK(umi_data_server_create_sqlite(file, &source));
  OK(umi_data_server_set(source, "original", "retained"));
  if (strcmp(name, "null-output") == 0) {
    CHECK(umi_data_backup_restore_checked(file, target, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto refused;
  }
  if (strcmp(name, "relative-source") == 0) {
    CHECK(umi_data_backup_restore_checked("source.sqlite", target, &report) ==
          UMI_STATUS_INVALID_ARGUMENT);
    goto refused;
  }
  if (strcmp(name, "relative-target") == 0) {
    CHECK(umi_data_backup_restore_checked(file, "target.sqlite", &report) ==
          UMI_STATUS_INVALID_ARGUMENT);
    goto refused;
  }
  if (strcmp(name, "same-source") == 0) {
    CHECK(umi_data_backup_restore_checked(file, file, &report) == UMI_STATUS_ALREADY_EXISTS);
    goto refused;
  }
  if (strcmp(name, "missing-source") == 0) {
    n = snprintf(selected, sizeof(selected), "%s.missing", file);
    CHECK(n > 0 && (size_t)n < sizeof(selected));
    CHECK(umi_data_backup_restore_checked(selected, target, &report) != UMI_STATUS_OK);
    goto refused;
  }
  if (strcmp(name, "unrelated") == 0) {
    OK(umi_data_server_execute(source,
                               "DROP TABLE umicom_kv; CREATE TABLE unrelated(value TEXT);"));
    CHECK(umi_data_backup_restore_checked(file, target, &report) == UMI_STATUS_INVALID_STATE);
    goto refused;
  }
  if (strcmp(name, "unhealthy") == 0) {
    OK(umi_data_server_execute(
        source, "CREATE TABLE damaged(value CHECK(value>0)); PRAGMA ignore_check_constraints=ON; "
                "INSERT INTO damaged VALUES(-1); PRAGMA ignore_check_constraints=OFF;"));
    /* Compare the preflight view before restore reserves its destination.
     * Retain all original assertions: evidence is not a substitute for health. */
    UmiDataServer *probe = NULL;
    UmiSqliteHealthReport writer_health = {0}, reader_health = {0};
    UmiStatus writer_status = umi_sqlite_health_read(source, &writer_health);
    UmiStatus reader_status = umi_data_server_open_sqlite_readonly(file, &probe);
    if (reader_status == UMI_STATUS_OK)
      reader_status = umi_sqlite_health_read(probe, &reader_health);
    fprintf(stderr, "restore preflight writer=%d sound=%d count=%zu reader=%d sound=%d count=%zu\n",
            (int)writer_status, writer_health.sound, writer_health.diagnostic_count,
            (int)reader_status, reader_health.sound, reader_health.diagnostic_count);
    umi_data_server_destroy(probe);
    CHECK(umi_data_backup_restore_checked(file, target, &report) == UMI_STATUS_INVALID_STATE);
    goto refused;
  }
  if (strcmp(name, "existing") == 0) {
    OK(umi_data_server_create_sqlite(target, &restored));
    OK(umi_data_server_set(restored, "destination", "kept"));
    CHECK(umi_data_backup_restore_checked(file, target, &report) == UMI_STATUS_ALREADY_EXISTS);
    char value[32];
    OK(umi_data_server_get(restored, "destination", value, sizeof(value)));
    CHECK(strcmp(value, "kept") == 0);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "source-wal") == 0) {
    OK(umi_data_server_execute(source, "PRAGMA journal_mode=WAL; PRAGMA wal_autocheckpoint=0;"));
    OK(umi_data_server_set(source, "latest", "in WAL"));
  }
  if (strcmp(name, "legacy") == 0)
    OK(umi_data_backup_restore(file, target));
  else {
    OK(umi_data_backup_restore_checked(file, target, &report));
    CHECK(report.page_count > 0 && report.sqlite_quick_check_passed);
  }
  OK(umi_data_server_create_sqlite(target, &restored));
  char value[32];
  OK(umi_data_server_get(restored, "original", value, sizeof(value)));
  CHECK(strcmp(value, "retained") == 0);
  if (strcmp(name, "source-wal") == 0) {
    OK(umi_data_server_get(restored, "latest", value, sizeof(value)));
    CHECK(strcmp(value, "in WAL") == 0);
  }
  OK(umi_data_server_set(restored, "original", "recovered edit"));
  OK(umi_data_server_get(source, "original", value, sizeof(value)));
  CHECK(strcmp(value, "retained") == 0);
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
  umi_data_server_destroy(restored);
  umi_data_server_destroy(source);
  return result;
}
