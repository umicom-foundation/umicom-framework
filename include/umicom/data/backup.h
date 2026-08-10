/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/backup.h
 *
 * PURPOSE:
 *   Declare Data Server backup creation and restoration for file-backed SQLite stores through Framework filesystem services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_BACKUP_H
#define UMICOM_DATA_BACKUP_H

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_data_backup_create(UmiDataServer *server,
                                 const char *backup_path);
UmiStatus umi_data_backup_restore(const char *backup_path,
                                  const char *database_path);

#ifdef __cplusplus
}
#endif

#endif
