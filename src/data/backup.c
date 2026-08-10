/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/backup.c
 *
 * PURPOSE:
 *   Implement checkpointed SQLite file backup and restore through Framework filesystem operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/backup.h"

#include "umicom/data/sqlite.h"
#include "umicom/platform/filesystem.h"

UmiStatus umi_data_backup_create(UmiDataServer *server,
                                 const char *backup_path)
{
    UmiStatus status;
    if (server == NULL || backup_path == NULL || backup_path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_data_server_backend(server) != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_sqlite_checkpoint(server);
    if (status != UMI_STATUS_OK) return status;
    return umi_fs_copy_file(umi_data_server_path(server), backup_path);
}

UmiStatus umi_data_backup_restore(const char *backup_path,
                                  const char *database_path)
{
    if (backup_path == NULL || database_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_fs_copy_file(backup_path, database_path);
}
