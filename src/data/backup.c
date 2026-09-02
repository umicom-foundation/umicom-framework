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
