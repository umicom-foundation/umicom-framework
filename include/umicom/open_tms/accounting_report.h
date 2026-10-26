/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/accounting_report.h
 *
 * PURPOSE:
 *   Capture a currency-separated spreadsheet report without changing local accounting state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_OPEN_TMS_ACCOUNTING_REPORT_H
#define UMICOM_OPEN_TMS_ACCOUNTING_REPORT_H
#include "umicom/open_tms/accounting_book.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Build one owned CSV snapshot. Caller destroys it with UmiCsvDocumentDestroy.
 * outReport must be distinct from book storage and is set to NULL on failure.
 * The input is borrowed for the call and must not change concurrently.
 *
 * Rows use the same twelve columns: record_kind, revision, approved_local,
 * posted_local, line_number, account, currency, reference, debit, credit,
 * difference, balanced_local. A batch row describes local state, line rows
 * retain capture order, and currency-total rows sort by three-letter code.
 * No mixed-currency grand total is produced. Empty numeric cells mean not
 * applicable, rather than zero. The local tolerance is 0.000001.
 *
 * Draft imbalance is reportable; a claimed approval/posting must balance.
 * Invalid fields, invalid UTF-8 and overflowing totals refuse the whole report.
 * Text uses the shared CSV formula-escaping policy. These double-valued practice
 * values are not an exact-money ledger, imported approval or posting authority.
 * This is a spreadsheet report, not an archive or lossless reimport format. */
UmiStatus umi_open_tms_accounting_book_report(
    const UmiOpenTmsAccountingBook *book, UmiCsvDocument **outReport);
#ifdef __cplusplus
}
#endif
#endif
