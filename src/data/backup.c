/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/backup.c
 *
 * PURPOSE:
 *   Implement checkpointed SQLite file backup and restore through Framework filesystem operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/backup.h"

#include "umicom/data/sqlite.h"
#include "umicom/platform/filesystem.h"

#include "umicom/platform/output_file.h"
#include "include/sqlite_backup_internal.h"
#include <string.h>

/* Checkpointing then copying a live main file can miss WAL commits or race
 * another writer. Replacing an existing database also bypasses its journal
 * ownership. SQLite snapshot copying to a new file replaces these bodies;
 * retain the earlier implementation for engineering review. */
#if 0
/*
 * Initialise data backup from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_data_backup_create(UmiDataServer *server,
                                 const char *backup_path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || backup_path == NULL || backup_path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_sqlite_checkpoint(server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_fs_copy_file(umi_data_server_path(server), backup_path);
}

/*
 * Provide the data backup restore operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_backup_restore(const char *backup_path,
                                  const char *database_path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (backup_path == NULL || database_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_fs_copy_file(backup_path, database_path);
}

#endif

UmiStatus umi_data_backup_create_checked(UmiDataServer *server,const char *backup_path,
                                        UmiDataBackupReport *out)
{
    if (server == NULL || backup_path == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(backup_path) >= sizeof out->destination) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiOutputFileValidatePath(backup_path);
    if (status != UMI_STATUS_OK) return status;
    UmiSqliteHealthReport health;
    status = umi_sqlite_health_read(server, &health);
    if (status != UMI_STATUS_OK) return status;
    if (!health.sound) return UMI_STATUS_INVALID_STATE;
    UmiOutputFile *reserved = NULL;
    status = UmiOutputFileCreate(backup_path, &reserved);
    if (status == UMI_STATUS_OK) status = UmiOutputFileClose(reserved);
    UmiOutputFileDestroy(reserved);
    if (status != UMI_STATUS_OK) return status;
    return umi_data_server_backup_reserved(server, backup_path, out);
}
UmiStatus umi_data_backup_restore_checked(const char *backup_path,const char *database_path,
                                         UmiDataBackupReport *out)
{
    if (backup_path == NULL || database_path == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(backup_path);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(database_path);
    if (status != UMI_STATUS_OK) return status;
    UmiDataServer *source = NULL;
    status = umi_data_server_open_sqlite_readonly(backup_path, &source);
    if (status == UMI_STATUS_OK) status = umi_data_backup_create_checked(source, database_path, out);
    umi_data_server_destroy(source);
    return status;
}
UmiStatus umi_data_backup_create(UmiDataServer *server,const char *backup_path)
{
    UmiDataBackupReport report;
    return umi_data_backup_create_checked(server, backup_path, &report);
}
UmiStatus umi_data_backup_restore(const char *backup_path,const char *database_path)
{
    UmiDataBackupReport report;
    return umi_data_backup_restore_checked(backup_path, database_path, &report);
}
