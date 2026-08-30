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

UmiStatus umi_sqlite_integrity_check(UmiDataServer *server)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    return umi_data_server_execute(server, "PRAGMA quick_check;");
}

UmiStatus umi_sqlite_checkpoint(UmiDataServer *server)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    return umi_data_server_execute(server, "PRAGMA wal_checkpoint(FULL);");
}

UmiStatus umi_sqlite_compact(UmiDataServer *server)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    return umi_data_server_execute(server, "VACUUM;");
}
