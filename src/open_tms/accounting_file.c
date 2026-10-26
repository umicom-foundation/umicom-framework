/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/accounting_file.c
 *
 * PURPOSE:
 *   Keep accounting filesystem access separate from the portable financial value library.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_file.h"
#include "umicom/platform/input_file.h"
#include <stdlib.h>
#include <string.h>

UmiStatus umi_open_tms_accounting_book_load(const char *path, UmiOpenTmsAccountingBook *outBook) {
    if (outBook == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(path, umi_open_tms_accounting_book_archive_limit(), &bytes, &size);
    if (status == UMI_STATUS_OK) {
        /* Bounded heap staging avoids placing a complete book on a worker's
         * small stack. The codec alone owns validation and atomic publication. */
        UmiOpenTmsAccountingBook *workspace = malloc(sizeof *workspace);
        if (workspace == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else {
            status = umi_open_tms_accounting_book_archive_decode_with_workspace(bytes, size, workspace, outBook);
            free(workspace);
        }
    }
    UmiInputFileFree(bytes);
    return status;
}
UmiStatus umi_open_tms_accounting_book_save_new(
    const char *path, const UmiOpenTmsAccountingBook *book, UmiOutputFileSnapshot *receipt) {
    if (receipt != NULL) memset(receipt, 0, sizeof *receipt);
    size_t size = 0U;
    unsigned char *bytes = NULL;
    UmiOutputFile *file = NULL;
    UmiStatus status = umi_open_tms_accounting_book_archive_encode(book, NULL, 0U, &size);
    if (status == UMI_STATUS_OK) {
        bytes = malloc(size);
        if (bytes == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else status = umi_open_tms_accounting_book_archive_encode(book, bytes, size, &size);
    }
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, bytes, size);
    if (file != NULL) {
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
        if (receipt != NULL) (void)UmiOutputFileRead(file, receipt);
    } else if (receipt != NULL) receipt->status = status;
    UmiOutputFileDestroy(file);
    free(bytes);
    return status;
}
