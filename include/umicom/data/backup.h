/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/backup.h
 *
 * PURPOSE:
 *   Declare Data Server backup creation and restoration for file-backed SQLite stores through Framework filesystem services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_BACKUP_H
#define UMICOM_DATA_BACKUP_H

#include "umicom/base/status.h"
#include <stdbool.h>
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A successful backup is a SQLite-consistent copy of main, including tables
 * beyond umicom_kv. The record count covers key/value rows only. quick_check is
 * structural evidence, not authentication or verification of application data. */
typedef struct UmiDataBackupReport {
    size_t page_count;
    size_t record_count;
    bool sqlite_quick_check_passed;
    char destination[1024];
} UmiDataBackupReport;
/* Source must be a SQLite Data Server with no active transaction. The explicit
 * absolute destination must be new, under a trusted existing parent. Existing
 * files are never opened or replaced. No directories or retries are created.
 * SQLite copies a consistent committed snapshot; source WAL files are not
 * copied as ordinary files. The destination is checked before reporting OK.
 * Output is unchanged on failure. A failure after exclusive creation may leave
 * an empty/partial new file for inspection; no previous file is deleted.
 * Serialize this call with source destruction and run it on a storage worker.
 * The parent directory must not be replaced concurrently: this is not a hostile
 * filesystem sandbox. No power-loss durability or encrypted backup claim. */
UmiStatus umi_data_backup_create_checked(UmiDataServer *server, const char *backup_path,
    UmiDataBackupReport *out);
/* Recover into a NEW selected database. Open the source read-only, validate it,
 * and copy it using the same SQLite snapshot path. This deliberately does not
 * replace an open or existing database. Review the new store before switching
 * an application to it; the original destination and source remain retained. */
UmiStatus umi_data_backup_restore_checked(const char *backup_path, const char *database_path,
    UmiDataBackupReport *out);
/* Existing status-only entry points delegate to the checked services. They now
 * require absolute new destinations and refuse overwrite instead of copying
 * onto an existing/open database. Their prior implementation is retained. */

/**
 * Initialise data backup from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_data_backup_create(UmiDataServer *server,
                                 const char *backup_path);
/**
 * Provide the data backup restore operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_backup_restore(const char *backup_path,
                                  const char *database_path);

#ifdef __cplusplus
}
#endif

#endif
