/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_review_admission.c
 *
 * PURPOSE:
 *   Exercise owned statement review with a deliberately unavailable serializer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "csv_fixture.h"
#include "umicom/bank_feed/csv_import.h"

static unsigned serialization_calls;

/* The included production code uses a controlled refusal in place of the
 * encoder. Keep the real CSV parser, statement arithmetic and ownership. */
#define UmiBankFeedArchiveEncode TestRefuseSerialization
#include "../../src/bank_feed/statement_archive.c"
#undef UmiBankFeedArchiveEncode

UmiStatus TestRefuseSerialization(const UmiBankFeed *feed, const UmiBankFeedArchiveInfo *info,
    char **bytes, size_t *length)
{
    (void)feed;
    (void)info;
    ++serialization_calls;
    *bytes = NULL;
    *length = 0U;
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    const char input[] = BANK_CSV_TEXT;
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCsvCreate(input, sizeof input - 1U, &feed, NULL) == UMI_STATUS_OK);
    const UmiBankStatementRequest request = {
        .account = "practice-account", .currency = "GBP",
        .firstDate = {2026, 10, 12}, .lastDate = {2026, 10, 12},
        .openingBalance = {10000, 2U}, .closingBalance = {11025, 2U}
    };
    UmiBankFeedArchiveInfo provenance = {.sourceFormat = UMI_BANK_FEED_SOURCE_STATEMENT_CSV};
    memset(provenance.sourceSha256, 'a', 64U);
    UmiBankStatementArchive *review = NULL;
    CHECK(UmiBankStatementArchiveCreate(feed, &request, &provenance, &review) == UMI_STATUS_OK);
    CHECK(serialization_calls == 0U);
    UmiBankStatementArchive *empty_digest_review = NULL;
    provenance.sourceSha256[0] = '\0';
    CHECK(UmiBankStatementArchiveCreate(feed, &request, &provenance, &empty_digest_review) == UMI_STATUS_OK);
    UmiBankStatementArchive *unknown_source_review = NULL;
    CHECK(UmiBankStatementArchiveCreate(feed, &request, NULL, &unknown_source_review) == UMI_STATUS_OK);
    UmiBankFeedDestroy(feed);
    memset(provenance.sourceSha256, 'b', 64U);

    UmiBankStatementArchiveSnapshot snapshot;
    CHECK(UmiBankStatementArchiveRead(empty_digest_review, &snapshot) == UMI_STATUS_OK);
    const char empty_digest[sizeof snapshot.source.sourceSha256] = {0};
    CHECK(memcmp(snapshot.source.sourceSha256, empty_digest, sizeof empty_digest) == 0);
    UmiBankStatementArchiveDestroy(empty_digest_review);
    CHECK(UmiBankStatementArchiveRead(unknown_source_review, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.source.sourceFormat == UMI_BANK_FEED_SOURCE_UNKNOWN &&
        memcmp(snapshot.source.sourceSha256, empty_digest, sizeof empty_digest) == 0);
    UmiBankStatementArchiveDestroy(unknown_source_review);
    CHECK(UmiBankStatementArchiveRead(review, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.result.balancesMatch && snapshot.result.bookedRows == 2U && snapshot.result.pendingRows == 1U);
    CHECK(snapshot.source.sourceFormat == UMI_BANK_FEED_SOURCE_STATEMENT_CSV &&
        snapshot.source.sourceSha256[0] == 'a' && strlen(snapshot.source.sourceSha256) == 64U);

    char *archive = NULL;
    size_t length = 123U;
    CHECK(UmiBankStatementArchiveEncode(review, &archive, &length) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(serialization_calls == 1U && archive == NULL && length == 0U);
    CHECK(UmiBankStatementArchiveRead(review, &snapshot) == UMI_STATUS_OK && snapshot.result.balancesMatch);
    UmiBankStatementArchiveDestroy(review);
    return 0;
}
