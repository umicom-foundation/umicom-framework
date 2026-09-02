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
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

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
