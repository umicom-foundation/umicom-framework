/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/bank_comparison/main.c
 *
 * PURPOSE:
 *   Demonstrate public C23 bank comparison services with invented imported data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/bank_feed/comparison.h"
#include "umicom/bank_feed/csv_import.h"
#include <stdio.h>
#include <string.h>
#define CSV_HEADER "account,transaction,reference,information,booking_date,currency,amount,status,mutability\n"
#define BEFORE CSV_HEADER "example,transfer-one,ref,Example movement,2026-10-12,GBP,-12.50,PDNG,Mutable\n"
#define AFTER CSV_HEADER "example,transfer-one,ref,Example movement,2026-10-12,GBP,-12.50000,BOOK,Immutable\n"
#define UNKNOWN CSV_HEADER "example,,ref,Unidentified movement,2026-10-12,GBP,1,BOOK,Immutable\n"
int main(int argc, char **argv) {
    if (argc != 2 || (strcmp(argv[1], "compare") != 0 && strcmp(argv[1], "missing-id") != 0))
        return 2;
    const bool unknown = strcmp(argv[1], "missing-id") == 0;
    const char *beforeText = unknown ? UNKNOWN : BEFORE;
    const char *afterText = unknown ? UNKNOWN : AFTER;
    UmiBankFeed *before = NULL, *after = NULL;
    UmiBankFeedComparison *comparison = NULL;
    UmiCsvDocument *csv = NULL;
    UmiStatus status = UmiBankFeedCsvCreate(beforeText, strlen(beforeText), &before, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiBankFeedCsvCreate(afterText, strlen(afterText), &after, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiBankFeedComparisonCreate(before, after, &comparison);
    /* The comparison owns both source snapshots; no source-tree helper is used
     * and releasing the imported feeds cannot invalidate its public row copies. */
    UmiBankFeedDestroy(before);
    UmiBankFeedDestroy(after);
    UmiBankFeedComparisonSummary summary = {0};
    if (status == UMI_STATUS_OK)
        status = UmiBankFeedComparisonRead(comparison, &summary);
    UmiBankFeedComparisonRow row = {0};
    if (status == UMI_STATUS_OK)
        status = UmiBankFeedComparisonAt(comparison, 0U, &row);
    if (status == UMI_STATUS_OK) {
        if (unknown) {
            if (summary.rows != 2U || summary.beforeUnidentified != 1U ||
                summary.afterUnidentified != 1U || summary.same != 0U)
                status = UMI_STATUS_INVALID_STATE;
        } else {
            const uint32_t expected = UMI_BANK_FEED_CHANGE_AMOUNT_TEXT |
                UMI_BANK_FEED_CHANGE_STATUS | UMI_BANK_FEED_CHANGE_MUTABILITY;
            int amountOrder = 1;
            status = UmiDecimalCompare(row.before.amount, row.after.amount, &amountOrder);
            if (status == UMI_STATUS_OK && (summary.changed != 1U ||
                row.changes != expected || amountOrder != 0))
                status = UMI_STATUS_INVALID_STATE;
        }
    }
    if (status == UMI_STATUS_OK)
        status = UmiBankFeedComparisonExportCsv(comparison, &csv);
    UmiBankFeedComparisonDestroy(comparison);
    if (status == UMI_STATUS_OK && UmiCsvDocumentRows(csv) != 4U)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        puts(unknown ? "Missing transaction IDs remained separate observations."
                     : "Exact amount stayed equal; spelling, status and mutability changes were retained.");
        puts(UmiCsvDocumentData(csv));
    } else fprintf(stderr, "Bank comparison failed: %s\n", umi_status_text(status));
    UmiCsvDocumentDestroy(csv);
    return status == UMI_STATUS_OK ? 0 : 1;
}
