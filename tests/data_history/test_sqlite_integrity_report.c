/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_sqlite_integrity_report.c
 *
 * PURPOSE:
 *   quick_check diagnostics are data rows, not failed SQL execution codes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * quick_check diagnostics are data rows, not failed SQL execution codes. */
#include "readonly_fixture.h"
#include "umicom/data/sqlite.h"
int main(int argc, char **argv) {
  const char *cases[] = {"healthy",     "constraint", "multiple",        "limit",
                         "readonly",    "memory",     "nested",          "null",
                         "null-output", "preserved",  "long-diagnostic", "foreign-key-scope",
                         "legacy", "checks-disabled"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  const char *name = argv[1];
  UmiDataServer *writer = NULL, *reader = NULL;
  char file[1024];
  UmiSqliteHealthReport report, sentinel;
  memset(&sentinel, 0x4a, sizeof(sentinel));
  report = sentinel;
  if (strcmp(name, "null") == 0) {
    CHECK(umi_sqlite_health_read(NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "memory") == 0) {
    OK(umi_data_server_create_memory(&writer));
    CHECK(umi_sqlite_health_read(writer, &report) == UMI_STATUS_UNAVAILABLE);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    goto cleanup;
  }
  CHECK(ro_path(argv[2], name, file, true));
  OK(umi_data_server_create_sqlite(file, &writer));
  OK(umi_data_server_set(writer, "original", "retained"));
  if (strcmp(name, "null-output") == 0) {
    CHECK(umi_sqlite_health_read(writer, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (strcmp(name, "nested") == 0 || strcmp(name, "preserved") == 0) {
    OK(umi_data_server_begin(writer));
    CHECK(umi_sqlite_health_read(writer, &report) == UMI_STATUS_BUSY);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    CHECK(umi_data_server_in_transaction(writer));
    OK(umi_data_server_rollback(writer));
    goto cleanup;
  }
  bool damaged = strcmp(name, "constraint") == 0 || strcmp(name, "multiple") == 0 ||
                 strcmp(name, "limit") == 0 || strcmp(name, "readonly") == 0 ||
                 strcmp(name, "long-diagnostic") == 0 || strcmp(name, "legacy") == 0 ||
                 strcmp(name, "checks-disabled") == 0;
  if (damaged) {
    if (strcmp(name, "long-diagnostic") == 0) {
      char table[650], sql[1800];
      memset(table, 'x', sizeof(table) - 1);
      table[sizeof(table) - 1] = 0;
      int n = snprintf(
          sql, sizeof(sql),
          "CREATE TABLE \"%s\"(value INTEGER CHECK(value>0)); PRAGMA ignore_check_constraints=ON; "
          "INSERT INTO \"%s\" VALUES(-1); PRAGMA ignore_check_constraints=OFF;",
          table, table);
      CHECK(n > 0 && (size_t)n < sizeof(sql));
      OK(umi_data_server_execute(writer, sql));
    } else {
      OK(umi_data_server_execute(writer,
                                 "CREATE TABLE damaged(value INTEGER CHECK(value>0)); PRAGMA "
                                 "ignore_check_constraints=ON; INSERT INTO damaged VALUES(-1);"));
      if (strcmp(name, "multiple") == 0)
        OK(umi_data_server_execute(writer, "INSERT INTO damaged VALUES(-2);"));
      if (strcmp(name, "limit") == 0)
        OK(umi_data_server_execute(writer,
                                   "WITH RECURSIVE n(x) AS (SELECT 1 UNION ALL SELECT x+1 FROM n "
                                   "WHERE x<120) INSERT INTO damaged SELECT -x FROM n;"));
      /* Keep the original fixture reset for review. The disabled-check case
       * deliberately leaves this connection policy active until admission. */
#if 0
      OK(umi_data_server_execute(writer, "PRAGMA ignore_check_constraints=OFF;"));
#endif
      if (strcmp(name, "checks-disabled") != 0)
        OK(umi_data_server_execute(writer, "PRAGMA ignore_check_constraints=OFF;"));
    }
  }
  if (strcmp(name, "foreign-key-scope") == 0)
    OK(umi_data_server_execute(
        writer, "PRAGMA foreign_keys=OFF; CREATE TABLE parent(id INTEGER PRIMARY KEY); CREATE "
                "TABLE child(id REFERENCES parent(id)); INSERT INTO child VALUES(42);"));
  if (strcmp(name, "checks-disabled") == 0) {
    CHECK(umi_sqlite_health_read(writer, &report) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&report, &sentinel, sizeof report) == 0);
    /* A second violating insert proves the health API did not silently change
     * the caller\'s policy. The subsequent explicit reset enables the scan. */
    OK(umi_data_server_execute(writer, "INSERT INTO damaged VALUES(-2);"));
    OK(umi_data_server_execute(writer, "PRAGMA ignore_check_constraints=OFF;"));
  }
  UmiDataServer *selected = writer;
  if (strcmp(name, "readonly") == 0) {
    OK(umi_data_server_open_sqlite_readonly(file, &reader));
    selected = reader;
  }
  OK(umi_sqlite_health_read(selected, &report));
  /* Keep the failing expectation intact. Read-only connection failures need
   * evidence from both handles; these diagnostics do not count as a repair. */
  if (report.sound != !damaged) {
    UmiSqliteHealthReport writer_report = {0};
    UmiStatus writer_status = umi_sqlite_health_read(writer, &writer_report);
    fprintf(stderr, "integrity case=%s readonly=%d sound=%d diagnostics=%zu first=%s\n", name,
            umi_data_server_is_readonly(selected), report.sound, report.diagnostic_count,
            report.first_diagnostic);
    fprintf(stderr, "writer status=%d sound=%d diagnostics=%zu first=%s\n", (int)writer_status,
            writer_report.sound, writer_report.diagnostic_count, writer_report.first_diagnostic);
  }
  CHECK(report.sound == !damaged);
  CHECK(umi_sqlite_integrity_check(selected) ==
        (damaged ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
  if (damaged) {
    CHECK(report.diagnostic_count > 0 && report.first_diagnostic[0]);
    if (strcmp(name, "multiple") == 0)
      CHECK(report.diagnostic_count == 2);
    if (strcmp(name, "limit") == 0)
      CHECK(report.diagnostic_count == 100 && report.diagnostic_limit_reached);
    if (strcmp(name, "long-diagnostic") == 0)
      CHECK(report.first_diagnostic_omitted);
  } else
    CHECK(report.diagnostic_count == 0 && !report.diagnostic_limit_reached &&
          !report.first_diagnostic_omitted && report.first_diagnostic[0] == 0);
  char value[32];
  OK(umi_data_server_get(selected, "original", value, sizeof(value)));
  CHECK(strcmp(value, "retained") == 0);
  CHECK(!umi_data_server_in_transaction(selected));
cleanup:
  if (writer && umi_data_server_in_transaction(writer))
    (void)umi_data_server_rollback(writer);
  umi_data_server_destroy(reader);
  umi_data_server_destroy(writer);
  return result;
}
