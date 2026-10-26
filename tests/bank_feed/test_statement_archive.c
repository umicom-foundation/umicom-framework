/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_archive.c
 *
 * PURPOSE:
 *   Exercise owned review round trips, scope validation and untrusted archive rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "csv_fixture.h"
#include "umicom/bank_feed/csv_import.h"
#include "umicom/bank_feed/statement_archive.h"
#include <limits.h>
#undef CHECK
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static UmiBankStatementRequest Request(void) {
    UmiBankStatementRequest request = {0};
    request.account = "practice-account"; request.currency = "GBP";
    CHECK(UmiBankStatementDate("2026-10-12", &request.firstDate) == UMI_STATUS_OK);
    request.lastDate = request.firstDate;
    request.openingBalance = (UmiDecimal){10000, 2U};
    request.closingBalance = (UmiDecimal){11025, 2U};
    return request;
}
static void Reject(const char *bytes, const char *find, const char *replacement) {
    char *bad = BankReplace(bytes, find, replacement);
    CHECK(bad != NULL);
    UmiBankStatementArchive *out = (UmiBankStatementArchive *)1;
    CHECK(UmiBankStatementArchiveDecode(bad, strlen(bad), &out) != UMI_STATUS_OK);
    CHECK(out == NULL);
    free(bad);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *input = BANK_CSV_TEXT
        "other-account,other,,,2026-10-12,GBP,999.00,BOOK,Immutable\n"
        "practice-account,euro,,,2026-10-12,EUR,999.00,BOOK,Immutable\n";
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCsvCreate(input, strlen(input), &feed, NULL) == UMI_STATUS_OK);
    UmiBankStatementRequest request = Request();
    UmiBankFeedArchiveInfo info = {.sourceFormat = UMI_BANK_FEED_SOURCE_STATEMENT_CSV};
    memset(info.sourceSha256, 'a', 64U);
    UmiBankStatementArchive *archive = NULL;
    if (!strcmp(mode, "mismatch")) request.closingBalance.coefficient++;
    if (!strcmp(mode, "mixed-scale")) {
        request.openingBalance = (UmiDecimal){100, 0U};
        request.closingBalance = (UmiDecimal){1102500, 4U};
    }
    if (!strcmp(mode, "negative")) {
        request.openingBalance = (UmiDecimal){-10000, 2U};
        request.closingBalance = (UmiDecimal){-8975, 2U};
    }
    if (!strcmp(mode, "empty-period") || !strcmp(mode, "integer-minimum")) {
        CHECK(UmiBankStatementDate("2026-10-13", &request.firstDate) == UMI_STATUS_OK);
        request.lastDate = request.firstDate;
        request.closingBalance = request.openingBalance;
        if (!strcmp(mode, "integer-minimum"))
            request.closingBalance = request.openingBalance = (UmiDecimal){INT64_MIN, 0U};
    }
    if (!strcmp(mode, "create-refusals")) {
        UmiBankStatementRequest bad = request;
        archive = (UmiBankStatementArchive *)1;
        bad.account = "unknown";
        CHECK(UmiBankStatementArchiveCreate(feed, &bad, &info, &archive) == UMI_STATUS_NOT_FOUND && archive == NULL);
        bad = request; bad.openingBalance = (UmiDecimal){INT64_MAX, 0U};
        CHECK(UmiBankStatementArchiveCreate(feed, &bad, &info, &archive) == UMI_STATUS_CAPACITY_EXCEEDED && archive == NULL);
        bad = request; bad.lastDate.day = 11U;
        CHECK(UmiBankStatementArchiveCreate(feed, &bad, &info, &archive) == UMI_STATUS_INVALID_ARGUMENT && archive == NULL);
        UmiBankFeedArchiveInfo invalid = info; memset(invalid.sourceSha256, 'a', sizeof invalid.sourceSha256);
        CHECK(UmiBankStatementArchiveCreate(feed, &request, &invalid, &archive) == UMI_STATUS_INVALID_ARGUMENT && archive == NULL);
    }
    CHECK(UmiBankStatementArchiveCreate(feed, &request, !strcmp(mode, "unknown-source") ? NULL : &info, &archive) == UMI_STATUS_OK);
    UmiBankFeedDestroy(feed); feed = NULL;
    UmiBankStatementArchiveSnapshot original;
    CHECK(UmiBankStatementArchiveRead(archive, &original) == UMI_STATUS_OK);
    CHECK(!strcmp(original.account, request.account) && !strcmp(original.currency, "GBP"));
    CHECK(original.openingBalance.coefficient == request.openingBalance.coefficient &&
        original.openingBalance.scale == request.openingBalance.scale);
    CHECK(original.closingBalance.coefficient == request.closingBalance.coefficient &&
        original.closingBalance.scale == request.closingBalance.scale);
    CHECK(original.result.balancesMatch == (strcmp(mode, "mismatch") != 0));
    CHECK(original.result.pendingRows == ((!strcmp(mode, "empty-period") || !strcmp(mode, "integer-minimum")) ? 0U : 1U));
    char *bytes = NULL;
    size_t length = 0U;
    CHECK(UmiBankStatementArchiveEncode(archive, &bytes, &length) == UMI_STATUS_OK);
    CHECK(length == strlen(bytes) && length <= UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES);
    CHECK(strstr(bytes, "\"result\"") == NULL && strstr(bytes, "\"balancesMatch\"") == NULL);
    UmiBankStatementArchiveDestroy(archive); archive = NULL;
    CHECK(UmiBankStatementArchiveDecode(bytes, length, &archive) == UMI_STATUS_OK);
    UmiBankStatementArchiveSnapshot restored;
    CHECK(UmiBankStatementArchiveRead(archive, &restored) == UMI_STATUS_OK);
    CHECK(restored.result.balancesMatch == original.result.balancesMatch);
    CHECK(restored.result.expectedClosing.coefficient == original.result.expectedClosing.coefficient);
    CHECK(restored.result.expectedClosing.scale == original.result.expectedClosing.scale);
    CHECK(restored.source.sourceFormat == original.source.sourceFormat);
    CHECK(!strcmp(restored.source.sourceSha256, original.source.sourceSha256));
    CHECK(UmiBankStatementArchiveCloneFeed(archive, &feed) == UMI_STATUS_OK);
    UmiBankStatementArchiveDestroy(archive); archive = NULL;
    UmiBankFeedSummary summary;
    CHECK(UmiBankFeedRead(feed, &summary) == UMI_STATUS_OK && summary.rows == 5U);
    UmiBankStatementResult recalculated;
    CHECK(UmiBankStatementReview(feed, &request, &recalculated) == UMI_STATUS_OK);
    CHECK(recalculated.difference.coefficient == original.result.difference.coefficient);
    UmiBankFeedDestroy(feed); feed = NULL;
    if (!strcmp(mode, "recalculate")) {
        char *changed = BankReplace(bytes, "\"closing_balance\":\"110.25\"", "\"closing_balance\":\"111.25\"");
        CHECK(changed != NULL);
        CHECK(UmiBankStatementArchiveDecode(changed, strlen(changed), &archive) == UMI_STATUS_OK);
        CHECK(UmiBankStatementArchiveRead(archive, &restored) == UMI_STATUS_OK);
        CHECK(!restored.result.balancesMatch && restored.result.difference.coefficient == 100);
        UmiBankStatementArchiveDestroy(archive); free(changed);
    } else if (!strcmp(mode, "root-unknown")) Reject(bytes, "\"version\":1", "\"version\":1,\"trusted\":true");
    else if (!strcmp(mode, "root-duplicate")) Reject(bytes, "\"currency\":\"GBP\"", "\"account\":\"practice-account\"");
    else if (!strcmp(mode, "root-missing")) Reject(bytes, ",\"currency\":\"GBP\"", "");
    else if (!strcmp(mode, "version")) Reject(bytes, "\"version\":1", "\"version\":2");
    else if (!strcmp(mode, "version-kind")) Reject(bytes, "\"version\":1", "\"version\":1.0");
    else if (!strcmp(mode, "format")) Reject(bytes, "umicom.bank-statement-review", "umicom.bank-observations");
    else if (!strcmp(mode, "invalid-date")) Reject(bytes, "\"first_date\":\"2026-10-12\"", "\"first_date\":\"2026-02-30\"");
    else if (!strcmp(mode, "reversed-period")) Reject(bytes, "\"last_date\":\"2026-10-12\"", "\"last_date\":\"2026-10-11\"");
    else if (!strcmp(mode, "unknown-account")) Reject(bytes, "\"account\":\"practice-account\"", "\"account\":\"missing\"");
    else if (!strcmp(mode, "currency-kind")) Reject(bytes, "\"currency\":\"GBP\"", "\"currency\":123");
    else if (!strcmp(mode, "noncanonical")) Reject(bytes, "\"opening_balance\":\"100.00\"", "\"opening_balance\":\"0100.00\"");
    else if (!strcmp(mode, "decimal-exponent")) Reject(bytes, "\"opening_balance\":\"100.00\"", "\"opening_balance\":\"1e2\"");
    else if (!strcmp(mode, "decimal-precision")) Reject(bytes, "\"opening_balance\":\"100.00\"", "\"opening_balance\":\"100.0000000000\"");
    else if (!strcmp(mode, "overflow")) Reject(bytes, "\"opening_balance\":\"100.00\"", "\"opening_balance\":\"9223372036854775807\"");
    else if (!strcmp(mode, "nested-unknown")) Reject(bytes, "\"previous_page\":false", "\"previous_page\":false,\"trusted\":true");
    else if (!strcmp(mode, "nested-duplicate")) Reject(bytes, "\"next_page\":false", "\"previous_page\":false");
    else if (!strcmp(mode, "nested-amount")) Reject(bytes, "\"amount\":\"12.50\"", "\"amount\":\"NaN\"");
    else if (!strcmp(mode, "nested-source")) Reject(bytes, "\"source_format\":2", "\"source_format\":-1");
    else if (!strcmp(mode, "pagination")) {
        char *changed = BankReplace(bytes, "\"previous_page\":false", "\"previous_page\":true");
        CHECK(changed != NULL);
        CHECK(UmiBankStatementArchiveDecode(changed, strlen(changed), &archive) == UMI_STATUS_OK);
        CHECK(UmiBankStatementArchiveRead(archive, &restored) == UMI_STATUS_OK && restored.result.hasPreviousPage);
        UmiBankStatementArchiveDestroy(archive); free(changed);
    } else if (!strcmp(mode, "truncated")) {
        archive = (UmiBankStatementArchive *)1;
        CHECK(UmiBankStatementArchiveDecode(bytes, length - 2U, &archive) != UMI_STATUS_OK && archive == NULL);
    } else if (!strcmp(mode, "embedded-nul")) {
        bytes[length / 2U] = '\0';
        CHECK(UmiBankStatementArchiveDecode(bytes, length, &archive) != UMI_STATUS_OK && archive == NULL);
    } else if (!strcmp(mode, "bounds")) {
        /* The byte bound is checked before attempting to read the supplied span. */
        CHECK(UmiBankStatementArchiveDecode(bytes, (size_t)UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES + 1U, &archive) == UMI_STATUS_CAPACITY_EXCEEDED && archive == NULL);
    } else if (!strcmp(mode, "invalid-api")) {
        char *outBytes = (char *)1; size_t outLength = 17U;
        CHECK(UmiBankStatementArchiveEncode(NULL, &outBytes, &outLength) == UMI_STATUS_INVALID_ARGUMENT && outBytes == NULL && outLength == 0U);
        UmiBankStatementArchiveSnapshot unchanged, before;
        memset(&unchanged, 0x5a, sizeof unchanged); before = unchanged;
        CHECK(UmiBankStatementArchiveRead(NULL, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&unchanged, &before, sizeof before) == 0);
        feed = (UmiBankFeed *)1;
        CHECK(UmiBankStatementArchiveCloneFeed(NULL, &feed) == UMI_STATUS_INVALID_ARGUMENT && feed == NULL);
        CHECK(UmiBankStatementArchiveCreate(NULL, NULL, NULL, &archive) == UMI_STATUS_INVALID_ARGUMENT && archive == NULL);
        CHECK(UmiBankStatementArchiveDecode(NULL, 0U, &archive) == UMI_STATUS_INVALID_ARGUMENT && archive == NULL);
        CHECK(UmiBankStatementArchiveDecode(bytes, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiBankStatementArchiveDestroy(NULL);
    } else {
        CHECK(!strcmp(mode, "roundtrip") || !strcmp(mode, "mismatch") || !strcmp(mode, "mixed-scale") ||
            !strcmp(mode, "negative") || !strcmp(mode, "empty-period") || !strcmp(mode, "integer-minimum") ||
            !strcmp(mode, "unknown-source") || !strcmp(mode, "create-refusals"));
    }
    UmiBankFeedArchiveFree(bytes);
    return 0;
}
