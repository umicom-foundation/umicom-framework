/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/csv_import.h
 * PURPOSE: Import an explicitly normalised statement CSV into shared bank observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_CSV_IMPORT_H
#define UMICOM_BANK_FEED_CSV_IMPORT_H
#include "umicom/bank_feed/transactions.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankFeedCsvIssue {
    size_t record, column;
} UmiBankFeedCsvIssue;
/** Decode the documented statement CSV, not an arbitrary bank's export.
 * The exact header is account,transaction,reference,information,booking_date,
 * currency,amount,status,mutability. Every record has nine columns.
 * Dates are YYYY-MM-DD; amounts are signed decimal text (negative means debit),
 * with 1..13 integer digits and at most 5 fraction digits. No rounding,
 * thousands separators, exponent notation or whitespace trimming is applied.
 * Status is BOOK or PDNG; mutability is Immutable or Mutable. Pending rows
 * remain mutable. Transaction IDs and reference/information may be empty.
 * Duplicate nonempty IDs within one account are rejected. UTF-8 and field
 * limits match bank observations; information may also contain TAB/CR/LF.
 * Byte/row limits are UMI_BANK_FEED_MAX_BYTES and UMI_BANK_FEED_MAX_ROWS.
 * Header-only input is a valid empty feed. No pagination evidence is available;
 * false page flags never establish completeness. No I/O or posting occurs.
 * Failure clears *out. Optional issue is cleared first, then receives one-based
 * logical record/column when known; zero means the failure was not localised.
 * Unsupported controls (including DEL), NUL and malformed UTF-8 are rejected
 * before records are decoded; those failures leave both coordinates zero.
 * The owned result is independent of input and uses the existing feed APIs. */
UmiStatus UmiBankFeedCsvCreate(const void *bytes, size_t length, UmiBankFeed **out,
                             UmiBankFeedCsvIssue *issue);
#ifdef __cplusplus
}
#endif
#endif
