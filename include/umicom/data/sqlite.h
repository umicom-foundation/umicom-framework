/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/sqlite.h
 *
 * PURPOSE:
 *   Declare SQLite-specific health, integrity, checkpoint and compaction operations without exposing sqlite3 handles to products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_SQLITE_H
#define UMICOM_DATA_SQLITE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the sqlite report data shared with callers of this public contract.
 */
typedef struct UmiSqliteReport {
    int available;
    int open;
    size_t key_value_records;
    const char *path;
} UmiSqliteReport;

/**
 * Provide the sqlite report operation used by this module and its client applications.
 */
UmiSqliteReport umi_sqlite_report(const UmiDataServer *server);
/**
 * Provide the sqlite integrity check operation used by this module and its client
 * applications.
 */
UmiStatus umi_sqlite_integrity_check(UmiDataServer *server);
/**
 * Provide the sqlite checkpoint operation used by this module and its client applications.
 */
UmiStatus umi_sqlite_checkpoint(UmiDataServer *server);
/**
 * Provide the sqlite compact operation used by this module and its client applications.
 */
UmiStatus umi_sqlite_compact(UmiDataServer *server);

#ifdef __cplusplus
}
#endif

#endif
