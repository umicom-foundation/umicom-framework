/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/sqlite.h
 *
 * PURPOSE:
 *   Declare SQLite-specific health, integrity, checkpoint and compaction operations without exposing sqlite3 handles to products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_SQLITE_H
#define UMICOM_DATA_SQLITE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSqliteReport {
    int available;
    int open;
    size_t key_value_records;
    const char *path;
} UmiSqliteReport;

UmiSqliteReport umi_sqlite_report(const UmiDataServer *server);
UmiStatus umi_sqlite_integrity_check(UmiDataServer *server);
UmiStatus umi_sqlite_checkpoint(UmiDataServer *server);
UmiStatus umi_sqlite_compact(UmiDataServer *server);

#ifdef __cplusplus
}
#endif

#endif
