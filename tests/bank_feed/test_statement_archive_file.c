/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_archive_file.c
 *
 * PURPOSE:
 *   Verify independent local reopening and distinguish exact source byte claims.
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
#include "umicom/platform/clock.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Statement archive file, line %d: %s\n", __LINE__, #condition); \
    goto finish; } } while (0)

static const char source[] =
    "account,transaction,reference,information,booking_date,currency,amount,status,mutability\n"
    "private-practice,credit,,,2026-01-15,GBP,12.50,BOOK,Immutable\n"
    "private-practice,pending,,,2026-01-15,GBP,-90.00,PDNG,Mutable\n";

/* Use a binary writer so a Windows newline conversion cannot mask the result
 * from the fixed digest calculated separately from Framework. */
static UmiStatus WriteFixture(const char *path, const char *bytes, size_t length)
{
    UmiOutputFile *file = NULL;
    UmiStatus status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK) {
        status = UmiOutputFileWrite(file, bytes, length);
        const UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
    }
    UmiOutputFileDestroy(file);
    return status;
}

int main(void)
{
    UmiBankFeed *feed = NULL;
    UmiBankStatementArchive *created = NULL, *opened = NULL, *legacy = NULL;
    unsigned char *first = NULL, *second = NULL;
    size_t firstLength = 0U, secondLength = 0U;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], leaf[64];
    char original[UMI_PATH_CAPACITY], changed[UMI_PATH_CAPACITY], archive[UMI_PATH_CAPACITY];
    char broken[UMI_PATH_CAPACITY], missing[UMI_PATH_CAPACITY];
    bool ownsFixture = false;
    int result = EXIT_FAILURE;
    UmiClock clock = umi_clock_system();
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof leaf, "umicom-statement-review-%llu",
        (unsigned long long)clock.monotonic_nanoseconds(&clock));
    CHECK(umi_path_join(temp, leaf, root, sizeof root) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    ownsFixture = true;
    CHECK(umi_path_join(root, "original.csv", original, sizeof original) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "changed.csv", changed, sizeof changed) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "review.json", archive, sizeof archive) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "broken.json", broken, sizeof broken) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "missing.csv", missing, sizeof missing) == UMI_STATUS_OK);
    CHECK(WriteFixture(original, source, sizeof source - 1U) == UMI_STATUS_OK);
    CHECK(WriteFixture(changed, source, sizeof source) == UMI_STATUS_OK);
    CHECK(WriteFixture(broken, "{", 1U) == UMI_STATUS_OK);

    const UmiBankFeedArchiveInfo info = {UMI_BANK_FEED_SOURCE_STATEMENT_CSV,
        "28f9b5b023213c90c5bd6d43fb37ed7c667098a01f7129cab701bae68b3b02b6"};
    const UmiFinancialDate date = {2026, 1, 15};
    const UmiBankStatementRequest request = {"private-practice", "GBP", date, date,
        {10000, 2U}, {11251, 2U}};
    CHECK(UmiBankFeedCsvCreate(source, sizeof source - 1U, &feed, NULL) == UMI_STATUS_OK);
    CHECK(UmiBankStatementArchiveCreate(feed, &request, &info, &created) == UMI_STATUS_OK);
    CHECK(UmiBankStatementArchiveCreate(feed, &request, NULL, &legacy) == UMI_STATUS_OK);
    UmiBankFeedDestroy(feed);
    feed = NULL;
    CHECK(UmiBankStatementArchiveSaveNewFile(NULL, missing) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!umi_fs_exists(missing));
    CHECK(UmiBankStatementArchiveSaveNewFile(created, archive) == UMI_STATUS_OK);
    CHECK(UmiInputFileRead(archive, UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES,
        &first, &firstLength) == UMI_STATUS_OK);
    CHECK(UmiBankStatementArchiveSaveNewFile(legacy, archive) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiInputFileRead(archive, UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES,
        &second, &secondLength) == UMI_STATUS_OK);
    CHECK(firstLength == secondLength && memcmp(first, second, firstLength) == 0);
    UmiBankStatementArchiveDestroy(created);
    created = NULL;

    CHECK(UmiBankStatementArchiveOpenFile(broken, &opened) == UMI_STATUS_PARSE_ERROR && opened == NULL);
    CHECK(UmiBankStatementArchiveOpenFile("review.json", &opened) != UMI_STATUS_OK && opened == NULL);
    CHECK(UmiBankStatementArchiveOpenFile(archive, &opened) == UMI_STATUS_OK);
    UmiBankStatementArchiveSnapshot snap;
    CHECK(UmiBankStatementArchiveRead(opened, &snap) == UMI_STATUS_OK);
    CHECK(snap.result.expectedClosing.coefficient == 11250 && snap.result.expectedClosing.scale == 2U);
    CHECK(snap.result.difference.coefficient == 1 && snap.result.difference.scale == 2U);
    CHECK(snap.result.bookedRows == 1U && snap.result.pendingRows == 1U && !snap.result.balancesMatch);
    CHECK(strcmp(snap.source.sourceSha256, info.sourceSha256) == 0);

    UmiBankStatementSourceCheck check = UMI_BANK_STATEMENT_SOURCE_DIGEST_UNRECORDED;
    CHECK(UmiBankStatementArchiveCheckSourceFile(opened, original, &check) == UMI_STATUS_OK &&
        check == UMI_BANK_STATEMENT_SOURCE_DIGEST_MATCH);
    CHECK(UmiBankStatementArchiveCheckSourceFile(opened, changed, &check) == UMI_STATUS_OK &&
        check == UMI_BANK_STATEMENT_SOURCE_DIGEST_MISMATCH);
    CHECK(UmiBankStatementArchiveCheckSourceFile(opened, missing, &check) != UMI_STATUS_OK &&
        check == UMI_BANK_STATEMENT_SOURCE_DIGEST_MISMATCH);
    CHECK(UmiBankStatementArchiveCheckSourceFile(legacy, "original.csv", &check) != UMI_STATUS_OK &&
        check == UMI_BANK_STATEMENT_SOURCE_DIGEST_MISMATCH);
    CHECK(UmiBankStatementArchiveCheckSourceFile(legacy, missing, &check) == UMI_STATUS_OK &&
        check == UMI_BANK_STATEMENT_SOURCE_DIGEST_UNRECORDED);
    puts("Statement review reopened independently, source claim compared, existing archive preserved");
    result = EXIT_SUCCESS;
finish:
    UmiInputFileFree(second);
    UmiInputFileFree(first);
    UmiBankStatementArchiveDestroy(legacy);
    UmiBankStatementArchiveDestroy(opened);
    UmiBankStatementArchiveDestroy(created);
    UmiBankFeedDestroy(feed);
    if (ownsFixture && umi_fs_remove_tree(root) != UMI_STATUS_OK) result = EXIT_FAILURE;
    return result;
}
