/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/statement_text.h
 * PURPOSE: Describe an account statement from the canonical revision-bounded journal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_OPERATIONS_STATEMENT_TEXT_H
#define UMICOM_BANK_OPERATIONS_STATEMENT_TEXT_H
#include "umicom/bank_operations/operations.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BANK_STATEMENT_TEXT_CAPACITY 131072U
/** Copy a readable, locale-independent practice statement over inclusive event
 * revisions. Earlier entries determine the opening balance; only entries in
 * the selected range are displayed. This is revision order, not a date filter.
 * The service is serially accessed; no reload, file write or state change occurs.
 * NULL output with capacity zero measures required size (including NUL) and
 * returns OK. A short buffer returns CAPACITY_EXCEEDED and clears output[0].
 * Other failures also clear supplied output and report required size zero.
 * Keep buffers separate from accountId and outRequired. Cached state may differ
 * from another process's newer commit; the report names its captured revision. */
UmiStatus UmiBankOperationsDescribeStatement(const UmiBankOperations *operations,
    const char *accountId, uint64_t firstRevision, uint64_t lastRevision,
    char *output, size_t capacity, size_t *outRequired);
#ifdef __cplusplus
}
#endif
#endif
