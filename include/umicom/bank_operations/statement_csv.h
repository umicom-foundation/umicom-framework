/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/statement_csv.h
 * PURPOSE: Export a copied local-practice statement as exact integer minor units.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_STATEMENT_CSV_H
#define UMICOM_BANK_STATEMENT_CSV_H
#include "umicom/bank_operations/operations.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Export a copied local-practice statement as exact integer minor units.
 * Uses the same inclusive revision range as UmiBankOperationsStatement.
 * The metadata row records currency, scale, opening/closing booked balances,
 * source revision and storage mode even for a range without entries. Holds
 * and unposted requests are excluded. Reversals remain separate entries.
 * No floating-point conversion, posting, reload or file write occurs.
 * Use on the operations owner's thread. The caller destroys successful
 * output, which survives owner destruction. Failure sets *outDocument=NULL. */
UmiStatus UmiBankOperationsExportStatementCsv(const UmiBankOperations *operations,
    const char *accountId, uint64_t firstRevision, uint64_t lastRevision,
    UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
