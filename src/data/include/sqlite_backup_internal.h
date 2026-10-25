/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/include/sqlite_backup_internal.h
 *
 * PURPOSE:
 *   Private destination ownership boundary for the public backup service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private destination ownership boundary for the public backup service. */
#ifndef UMICOM_DATA_SQLITE_BACKUP_INTERNAL_H
#define UMICOM_DATA_SQLITE_BACKUP_INTERNAL_H
#include "umicom/data/backup.h"
/* Caller exclusively created and closed this new empty file in a trusted
 * directory. Only backup.c may route a user-selected destination here. */
UmiStatus umi_data_server_backup_reserved(UmiDataServer *server, const char *destination,
                                          UmiDataBackupReport *out);
#endif
