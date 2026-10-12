/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/transactions.h
 * PURPOSE: Inspect imported bank transactions without posting them to an account.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_TRANSACTIONS_H
#define UMICOM_BANK_FEED_TRANSACTIONS_H
#include "umicom/base/csv_document.h"
#include "umicom/finance/decimal.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BANK_FEED_MAX_BYTES (4U * 1024U * 1024U)
#define UMI_BANK_FEED_MAX_ROWS 1024U
typedef struct UmiBankFeed UmiBankFeed;
typedef struct UmiBankFeedRow {
  char account[161], transaction[841], reference[841], information[2001];
  char bookingTime[40], currency[4], amountText[24];
  UmiDecimal amount;
  bool credit, booked, mutableRecord;
} UmiBankFeedRow;
typedef struct UmiBankFeedSummary {
  size_t rows, missingIdentifiers;
  bool hasPreviousPage, hasNextPage;
} UmiBankFeedSummary;
typedef struct UmiBankFeedTotals {
  size_t bookedRows, pendingRows, mutableRows, unidentifiedRows;
  UmiDecimal bookedCredits, bookedDebits, bookedNet;
  UmiDecimal pendingCredits, pendingDebits, pendingNet;
} UmiBankFeedTotals;
/** Decode one saved Open Banking transaction response, not an authenticated
 * connection. Supports Data.Transaction, Credit/Debit and BOOK/PDNG (also
 * Booked/Pending). Selected fields are copied; unknown extensions are discarded.
 * Amounts follow the bank format: 1..13 integer and at most 5 fractional digits.
 * Duplicate nonempty transaction IDs in one account are rejected, never summed.
 * Missing IDs remain separate observations. Limits above bound memory use.
 * No I/O occurs. Failure clears *out; serialise calls on each retained feed. */
UmiStatus UmiBankFeedCreate(const void *bytes, size_t length, UmiBankFeed **out);
/** Release the imported observations. NULL is accepted. */
void UmiBankFeedDestroy(UmiBankFeed *feed);
/** Copy page counts and pagination flags. No flag proves a complete statement.
 * Failure leaves output unchanged. */
UmiStatus UmiBankFeedRead(const UmiBankFeed *feed, UmiBankFeedSummary *out);
/** Copy one original-order row. Failure leaves output unchanged. */
UmiStatus UmiBankFeedAt(const UmiBankFeed *feed, size_t index, UmiBankFeedRow *out);
/** Sum only one exact account and currency, retaining booked and pending totals
 * separately. Net is credits less debits, not an opening/closing balance.
 * Mutable booked entries still belong to the booked group and are counted.
 * Checked decimal arithmetic never converts or combines different currencies.
 * No matching rows returns NOT_FOUND; any failure leaves output unchanged. */
UmiStatus UmiBankFeedSum(const UmiBankFeed *feed, const char *account, const char *currency,
                         UmiBankFeedTotals *out);
/** Export all imported rows plus one page-metadata row to an owned CSV document.
 * The common CSV writer quotes text and escapes spreadsheet formula prefixes.
 * Account IDs and descriptions are private data. The caller chooses where to
 * save the result. This performs no file write and clears *out on failure. */
UmiStatus UmiBankFeedExportCsv(const UmiBankFeed *feed, UmiCsvDocument **out);
#ifdef __cplusplus
}
#endif
#endif
