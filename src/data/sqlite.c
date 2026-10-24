/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/sqlite.c
 *
 * PURPOSE:
 *   Implement safe SQLite health operations through Data Server SQL execution without leaking vendor handles across the Framework boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/sqlite.h"

/* Provide the sqlite report operation used by this module and its client applications. */
UmiSqliteReport umi_sqlite_report(const UmiDataServer *server)
{
    UmiSqliteReport report;
#ifdef UMICOM_HAS_SQLITE
    report.available = 1;
#else
    report.available = 0;
#endif
    report.open = server != NULL &&
                  umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE;
    report.key_value_records = server != NULL ? umi_data_server_count(server) : 0U;
    report.path = server != NULL ? umi_data_server_path(server) : "";
    return report;
}

/*
 * Provide the sqlite integrity check operation used by this module and its client
 * applications.
 */
/* Executing a PRAGMA successfully does not mean its diagnostic rows report a healthy store. The checked reader consumes and interprets those rows. The prior implementation is retained for review. */
#if 0
UmiStatus umi_sqlite_integrity_check(UmiDataServer *server)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    return umi_data_server_execute(server, "PRAGMA quick_check;");
}
#endif
UmiStatus umi_sqlite_integrity_check(UmiDataServer *server)
{
    UmiSqliteHealthReport report;
    UmiStatus status = umi_sqlite_health_read(server, &report);
    if (status != UMI_STATUS_OK) return status;
    return report.sound ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Provide the sqlite checkpoint operation used by this module and its client applications. */
/* Checkpoint contention is reported by SQLite result state. The shared checkpoint operation now returns BUSY instead of discarding that information. The prior implementation is retained for review. */
#if 0
UmiStatus umi_sqlite_checkpoint(UmiDataServer *server)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    return umi_data_server_execute(server, "PRAGMA wal_checkpoint(FULL);");
}
#endif
UmiStatus umi_sqlite_checkpoint(UmiDataServer *server)
{
    UmiSqliteCheckpointReport report;
    return umi_sqlite_checkpoint_full(server, &report);
}

/* Provide the sqlite compact operation used by this module and its client applications. */
UmiStatus umi_sqlite_compact(UmiDataServer *server)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    return umi_data_server_execute(server, "VACUUM;");
}
