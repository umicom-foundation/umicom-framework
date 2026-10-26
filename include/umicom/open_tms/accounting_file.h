/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/accounting_file.h
 *
 * PURPOSE:
 *   Save new private accounting archives and load independent, validated practice values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_OPEN_TMS_ACCOUNTING_FILE_H
#define UMICOM_OPEN_TMS_ACCOUNTING_FILE_H
#include "umicom/open_tms/accounting_archive.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C" {
#endif
/* These blocking file operations belong on a worker. Paths follow the platform
 * input/output contracts: absolute ordinary files in a trusted directory, not
 * a sandbox boundary. No directory is created and no existing file is replaced.
 * No encryption, authentication or external financial authority is conferred.
 *
 * Load validates a bounded archive into heap workspace before changing outBook.
 * Any failure leaves that independent caller-owned book unchanged. Replacing a
 * live book requires explicit review and revocation of its old transition reviews.
 *
 * Imported approval and posting flags are historical values only; load does not
 * grant approval or posting authority.
 *
 * Save serializes the complete book before exclusive creation. The optional
 * receipt records bytes accepted and close status. A write failure retains its
 * partial file for inspection; choose a different path to retry. Receipt and
 * book must not overlap. Inputs may not be mutated during these synchronous calls. */
UmiStatus umi_open_tms_accounting_book_load(
    const char *path, UmiOpenTmsAccountingBook *outBook);
UmiStatus umi_open_tms_accounting_book_save_new(
    const char *path, const UmiOpenTmsAccountingBook *book, UmiOutputFileSnapshot *receipt);
#ifdef __cplusplus
}
#endif
#endif
