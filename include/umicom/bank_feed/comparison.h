/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/comparison.h
 *
 * PURPOSE:
 *   Compare owned imported bank observations without changing account records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BANK_FEED_COMPARISON_H
#define UMICOM_BANK_FEED_COMPARISON_H
#include "umicom/bank_feed/transactions.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBankFeedComparison UmiBankFeedComparison;
typedef enum UmiBankFeedComparisonKind {
    UMI_BANK_FEED_SAME,
    UMI_BANK_FEED_CHANGED,
    UMI_BANK_FEED_BEFORE_ONLY,
    UMI_BANK_FEED_AFTER_ONLY,
    UMI_BANK_FEED_BEFORE_UNIDENTIFIED,
    UMI_BANK_FEED_AFTER_UNIDENTIFIED
} UmiBankFeedComparisonKind;

/* These flags describe changed observations, not authorisation to post or
 * reverse money. Amount spelling is separate from its exact decimal value. */
#define UMI_BANK_FEED_CHANGE_AMOUNT       UINT32_C(1)
#define UMI_BANK_FEED_CHANGE_AMOUNT_TEXT  UINT32_C(2)
#define UMI_BANK_FEED_CHANGE_CURRENCY     UINT32_C(4)
#define UMI_BANK_FEED_CHANGE_DIRECTION    UINT32_C(8)
#define UMI_BANK_FEED_CHANGE_STATUS       UINT32_C(16)
#define UMI_BANK_FEED_CHANGE_MUTABILITY   UINT32_C(32)
#define UMI_BANK_FEED_CHANGE_BOOKING_TIME UINT32_C(64)
#define UMI_BANK_FEED_CHANGE_REFERENCE    UINT32_C(128)
#define UMI_BANK_FEED_CHANGE_INFORMATION  UINT32_C(256)

typedef struct UmiBankFeedComparisonSummary {
    UmiBankFeedSummary before, after;
    size_t rows, same, changed, beforeOnly, afterOnly;
    size_t beforeUnidentified, afterUnidentified;
} UmiBankFeedComparisonSummary;

typedef struct UmiBankFeedComparisonRow {
    UmiBankFeedComparisonKind kind;
    uint32_t changes;
    /* SIZE_MAX means absent. Present indices refer to the respective original
     * imported page. An absent side's row value is all zero. */
    size_t beforeIndex, afterIndex;
    UmiBankFeedRow before, after;
} UmiBankFeedComparisonRow;

/** Match exact account ID plus nonempty transaction ID. Missing IDs NEVER
 * match, including two otherwise identical observations. Compare every retained
 * field: equal decimal values with different spelling still set AMOUNT_TEXT.
 * Booking times compare their original text; no timezone inference occurs.
 * Output follows original before order, followed by unmatched after rows in
 * their original order. At most twice UMI_BANK_FEED_MAX_ROWS entries exist.
 * Both inputs are borrowed only during this call; result owns complete copies.
 * Failure clears *out. No I/O, ledger mutation, currency conversion or network
 * action occurs. Use on one owner thread; inputs must not change during capture.
 * One-sided rows mean only absent from the OTHER IMPORT. Page flags do not
 * establish completeness, authenticity, chronological order or deletion. */
UmiStatus UmiBankFeedComparisonCreate(const UmiBankFeed *before,
    const UmiBankFeed *after, UmiBankFeedComparison **out);
void UmiBankFeedComparisonDestroy(UmiBankFeedComparison *comparison);
/** Copy immutable summary or result row; failure preserves caller output. */
UmiStatus UmiBankFeedComparisonRead(const UmiBankFeedComparison *comparison,
    UmiBankFeedComparisonSummary *out);
UmiStatus UmiBankFeedComparisonAt(const UmiBankFeedComparison *comparison,
    size_t index, UmiBankFeedComparisonRow *out);
/** Export a metadata row and one row for each present side of each comparison.
 * Original fields and both imports' page flags remain on each data row. Text
 * passes through the shared spreadsheet-formula escape policy. No file is
 * written. Private account/reference data remains private. The bounded CSV
 * may refuse an expansion larger than UMI_CSV_MAX_BYTES; failure clears *out. */
UmiStatus UmiBankFeedComparisonExportCsv(const UmiBankFeedComparison *comparison,
    UmiCsvDocument **out);
/** Stable human-readable names for views and reports. Unknown kinds return NULL. */
const char *UmiBankFeedComparisonKindName(UmiBankFeedComparisonKind kind);
#ifdef __cplusplus
}
#endif
#endif
