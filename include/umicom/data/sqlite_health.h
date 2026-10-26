/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/sqlite_health.h
 *
 * PURPOSE:
 *   Check SQLite result rows rather than treating statement execution as health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Check SQLite result rows rather than treating statement execution as health. */
#ifndef UMICOM_DATA_SQLITE_HEALTH_H
#define UMICOM_DATA_SQLITE_HEALTH_H
#include "umicom/data/data_server.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SQLITE_HEALTH_DIAGNOSTIC_LIMIT 100U
typedef struct UmiSqliteHealthReport {
  bool sound;
  bool diagnostic_limit_reached;
  bool first_diagnostic_omitted;
  size_t diagnostic_count;
  char first_diagnostic[512];
} UmiSqliteHealthReport;
typedef struct UmiSqliteCheckpointReport {
  bool wal_mode;
  int log_frames;
  int checkpointed_frames;
} UmiSqliteCheckpointReport;
/* Run main.quick_check(100) and consume all of its result rows under the Data
 * Server lock. OK means the check completed: inspect sound to decide whether
 * SQLite reported structural problems. A sound result is exactly one "ok" row.
 * The count is bounded, not proof that no further problems exist. An oversized
 * first diagnostic is replaced by a fixed explanation and marked omitted.
 * This does not check foreign keys, record checksums, access rights or backups.
 * INVALID_STATE rejects connections with ignore_check_constraints enabled,
 * leaving both output and connection policy unchanged. Disable that pragma
 * explicitly before requesting a complete CHECK-constraint scan.
 * Read-only connections also scan a private writable SQLite temporary snapshot
 * because SQLite may omit CHECK expressions from a read-only schema. The source
 * remains read-only; a private read transaction gives both scans one snapshot.
 * This may require temporary disk space proportional to the database size.
 * SQLite removes its unnamed temporary database on close. A failed copy or
 * cleanup returns an error, never a healthy report. Existing owned transactions are BUSY;
 * no operation silently commits, rolls back or includes pending caller edits.
 * Output is unchanged for admission, engine or completion errors. Runs on the
 * caller's worker; the diagnostic bound does not bound the database scan time. */
UmiStatus umi_sqlite_health_read(UmiDataServer *server, UmiSqliteHealthReport *out);
/* Complete a FULL checkpoint of main. Return BUSY for contention or an active
 * transaction, PERMISSION_DENIED for read-only mode. Output changes only on
 * success. Non-WAL databases succeed with wal_mode=false and frame counts -1.
 * The connection's busy timeout applies; no automatic retry loop is introduced. */
UmiStatus umi_sqlite_checkpoint_full(UmiDataServer *server, UmiSqliteCheckpointReport *out);
#ifdef __cplusplus
}
#endif
#endif
