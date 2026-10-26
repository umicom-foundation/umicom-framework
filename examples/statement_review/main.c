/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/statement_review/main.c
 *
 * PURPOSE:
 *   Demonstrate an owned statement comparison round trip through the public SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/bank_feed/csv_import.h"
#include "umicom/bank_feed/statement_archive.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2);
    bool mismatch = strcmp(argv[1], "mismatch") == 0;
    CHECK(mismatch || strcmp(argv[1], "roundtrip") == 0);
    const char input[] =
        "account,transaction,reference,information,booking_date,currency,amount,status,mutability\n"
        "sample,credit,,,2026-01-15,GBP,12.50,BOOK,Immutable\n"
        "sample,pending,,,2026-01-15,GBP,-90.00,PDNG,Mutable\n";
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCsvCreate(input, sizeof input - 1U, &feed, NULL) == UMI_STATUS_OK);
    UmiBankStatementRequest request = {0};
    request.account = "sample"; request.currency = "GBP";
    CHECK(UmiBankStatementDate("2026-01-15", &request.firstDate) == UMI_STATUS_OK);
    request.lastDate = request.firstDate;
    request.openingBalance = (UmiDecimal){10000, 2U};
    request.closingBalance = (UmiDecimal){mismatch ? 11251 : 11250, 2U};
    UmiBankStatementArchive *review = NULL;
    CHECK(UmiBankStatementArchiveCreate(feed, &request, NULL, &review) == UMI_STATUS_OK);
    UmiBankFeedDestroy(feed);
    char *bytes = NULL; size_t length = 0U;
    CHECK(UmiBankStatementArchiveEncode(review, &bytes, &length) == UMI_STATUS_OK);
    UmiBankStatementArchiveDestroy(review); review = NULL;
    CHECK(UmiBankStatementArchiveDecode(bytes, length, &review) == UMI_STATUS_OK);
    UmiBankFeedArchiveFree(bytes);
    UmiBankStatementArchiveSnapshot snapshot;
    CHECK(UmiBankStatementArchiveRead(review, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.result.balancesMatch == !mismatch);
    CHECK(snapshot.result.pendingRows == 1U && snapshot.result.bookedRows == 1U);
    CHECK(snapshot.result.expectedClosing.coefficient == 11250 && snapshot.result.expectedClosing.scale == 2U);
    CHECK(snapshot.source.sourceFormat == UMI_BANK_FEED_SOURCE_UNKNOWN);
    char difference[UMI_DECIMAL_TEXT_MAX];
    CHECK(UmiDecimalFormat(snapshot.result.difference, difference, sizeof difference) == UMI_STATUS_OK);
    printf("Reopened sample comparison: difference %s; pending rows excluded: %zu\n",
        difference, snapshot.result.pendingRows);
    UmiBankStatementArchiveDestroy(review);
    return 0;
}
