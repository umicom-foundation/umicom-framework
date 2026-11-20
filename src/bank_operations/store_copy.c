/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/store_copy.c
 *
 * PURPOSE:
 *   Copy an explicitly selected banking simulation store and replay the result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/data/backup.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>

/* Borrow the read-only Data Server for the same typed history replay used by
 * the editable banking owner. The staged projection has no public lifetime. */
static UmiStatus ReadStoreCounts(UmiDataServer *server, UmiBankCounts *out_counts)
{
    UmiBankOperations projection = {.server = server};
    UmiStatus status = BankRepositoryLoad(&projection, &projection.state);
    if (status == UMI_STATUS_OK) *out_counts = projection.state->counts;
    free(projection.state);
    return status;
}

UmiStatus UmiBankOperationsCopyExistingSqlite(const char *source_path, const char *destination_path,
                                             UmiBankCounts *out_copied_counts)
{
    if (out_copied_counts == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Check both user-owned locations before I/O. The Platform validator
     * admits ordinary absolute filenames without guessing a working folder. */
    UmiStatus status = UmiOutputFileValidatePath(source_path);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(destination_path);
    if (status != UMI_STATUS_OK) return status;

    UmiDataServer *source = NULL;
    UmiDataServer *copied = NULL;
    UmiBankCounts counts;
    UmiDataBackupReport backup;
    /* Validate the original history without opening it for writing. SQLite's
     * backup owner captures a consistent snapshot and exclusively reserves a
     * new destination. It never copies the live database as an ordinary file. */
    status = umi_data_server_open_sqlite_readonly(source_path, &source);
    if (status == UMI_STATUS_OK) status = ReadStoreCounts(source, &counts);
    if (status == UMI_STATUS_OK) status = umi_data_backup_create_checked(source, destination_path, &backup);
    umi_data_server_destroy(source);

    /* Validate the actual copy, since its committed snapshot may be newer
     * than the original admission read. Return only counts from that copy. */
    if (status == UMI_STATUS_OK) status = umi_data_server_open_sqlite_readonly(destination_path, &copied);
    if (status == UMI_STATUS_OK) status = ReadStoreCounts(copied, &counts);
    umi_data_server_destroy(copied);
    if (status == UMI_STATUS_OK) *out_copied_counts = counts;
    return status;
}
