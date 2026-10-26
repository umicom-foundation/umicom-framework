/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_file.c
 *
 * PURPOSE:
 *   Check bounded file admission, complete round trips and preservation of existing evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/fx_forward_file.h"
#include "../build_log/fixture.h"
static void Populate(UmiOpenTmsFxForwardBook *book) {
    umi_open_tms_fx_forward_book_init(book);
    UmiOpenTmsFxForward trade = {0};
    strcpy(trade.tradeId, "synthetic-forward"); strcpy(trade.accountId, "practice");
    strcpy(trade.counterpartyId, "synthetic"); strcpy(trade.baseCurrency, "GBP"); strcpy(trade.quoteCurrency, "USD");
    trade.baseNotional = 1250.5; trade.forwardRate = 1.1; trade.marketForwardRate = 0.0;
    trade.maturityMilliseconds = UINT64_MAX; trade.version = UINT64_MAX;
    trade.state = UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING;
    CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
}
static void Write(const char *path, const void *bytes, size_t size) {
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, size) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
}
static void Same(const UmiOpenTmsFxForwardBook *a, const UmiOpenTmsFxForwardBook *b) {
    size_t an = 0U, bn = 0U;
    CHECK(umi_open_tms_fx_forward_book_archive_encode(a, NULL, 0U, &an) == UMI_STATUS_OK);
    CHECK(umi_open_tms_fx_forward_book_archive_encode(b, NULL, 0U, &bn) == UMI_STATUS_OK && an == bn);
    unsigned char *ab = malloc(an), *bb = malloc(bn); CHECK(ab && bb);
    CHECK(umi_open_tms_fx_forward_book_archive_encode(a, ab, an, &an) == UMI_STATUS_OK);
    CHECK(umi_open_tms_fx_forward_book_archive_encode(b, bb, bn, &bn) == UMI_STATUS_OK);
    CHECK(memcmp(ab, bb, an) == 0); free(bb); free(ab);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "forward-caf\xc3\xa9.umiv");
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book), *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    CHECK(book && out && before); Populate(book);
    memset(out, 0xA5, sizeof *out); memcpy(before, out, sizeof *out);
    UmiOutputFileSnapshot receipt;
    memset(&receipt, 0xA5, sizeof receipt);
    UmiStatus status;
    if (!strcmp(mode, "roundtrip") || !strcmp(mode, "empty") || !strcmp(mode, "full") || !strcmp(mode, "no-receipt")) {
        if (!strcmp(mode, "empty")) umi_open_tms_fx_forward_book_init(book);
        if (!strcmp(mode, "full")) {
            UmiOpenTmsFxForward trade = book->trades[0];
            for (size_t i = book->count; i < UMI_OPEN_TMS_FX_FORWARD_CAPACITY; ++i) {
                (void)snprintf(trade.tradeId, sizeof trade.tradeId, "trade-%zu", i);
                CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
            }
        }
        CHECK(umi_open_tms_fx_forward_book_save_new(path, book, !strcmp(mode, "no-receipt") ? NULL : &receipt) == UMI_STATUS_OK);
        if (strcmp(mode, "no-receipt")) {
            CHECK(receipt.closed && receipt.bytes_written > 0U && receipt.status == UMI_STATUS_OK);
            CHECK(strcmp(receipt.path, path) == 0);
        }
        CHECK(umi_open_tms_fx_forward_book_load(path, out) == UMI_STATUS_OK); Same(book, out);
    } else if (!strcmp(mode, "existing")) {
        CHECK(umi_open_tms_fx_forward_book_save_new(path, book, &receipt) == UMI_STATUS_OK);
        size_t originalSize = 0U; unsigned char *original = FixtureRead(path, &originalSize);
        book->trades[0].baseNotional += 1.0;
        CHECK(umi_open_tms_fx_forward_book_save_new(path, book, &receipt) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(receipt.status == UMI_STATUS_ALREADY_EXISTS && receipt.path[0] == '\0' && receipt.bytes_written == 0U);
        size_t actualSize = 0U; unsigned char *actual = FixtureRead(path, &actualSize);
        CHECK(actualSize == originalSize && memcmp(original, actual, actualSize) == 0);
        free(actual); free(original);
    } else if (!strcmp(mode, "invalid-book") || !strcmp(mode, "null-book")) {
        book->count = UMI_OPEN_TMS_FX_FORWARD_CAPACITY + 1U;
        status = umi_open_tms_fx_forward_book_save_new(path, !strcmp(mode, "null-book") ? NULL : book, &receipt);
        CHECK(status != UMI_STATUS_OK && receipt.status == status && receipt.path[0] == '\0');
        /* Exclusive creation proves serialization failure did not leave a file. */
        Write(path, "", 0U);
    } else if (!strcmp(mode, "save-relative") || !strcmp(mode, "save-null") ||
               !strcmp(mode, "save-directory") || !strcmp(mode, "save-missing-parent")) {
        if (!strcmp(mode, "save-missing-parent")) FixturePath(path, root, "missing/forward.umiv");
        const char *destination = !strcmp(mode, "save-relative") ? "relative.umiv" :
            !strcmp(mode, "save-null") ? NULL : !strcmp(mode, "save-directory") ? root : path;
        status = umi_open_tms_fx_forward_book_save_new(destination, book, &receipt);
        CHECK(status != UMI_STATUS_OK && receipt.status == status && receipt.path[0] == '\0' && receipt.bytes_written == 0U);
    } else if (!strcmp(mode, "load-null-output")) {
        CHECK(umi_open_tms_fx_forward_book_load(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "missing") || !strcmp(mode, "load-relative") || !strcmp(mode, "load-null") ||
               !strcmp(mode, "directory") || !strcmp(mode, "malformed") || !strcmp(mode, "zero") ||
               !strcmp(mode, "oversize") || !strcmp(mode, "checksum") || !strcmp(mode, "trailing")) {
        if (!strcmp(mode, "malformed")) Write(path, "not an archive", 14U);
        if (!strcmp(mode, "zero")) Write(path, "", 0U);
        if (!strcmp(mode, "oversize")) {
            size_t limit = umi_open_tms_fx_forward_book_archive_limit();
            CHECK(limit > 0U && limit < 1048576U);
            unsigned char *bytes = calloc(limit + 1U, 1U); CHECK(bytes);
            Write(path, bytes, limit + 1U); free(bytes);
        }
        if (!strcmp(mode, "checksum") || !strcmp(mode, "trailing")) {
            size_t size = 0U;
            CHECK(umi_open_tms_fx_forward_book_archive_encode(book, NULL, 0U, &size) == UMI_STATUS_OK);
            unsigned char *bytes = calloc(size + 1U, 1U); CHECK(bytes);
            CHECK(umi_open_tms_fx_forward_book_archive_encode(book, bytes, size, &size) == UMI_STATUS_OK);
            if (!strcmp(mode, "checksum")) bytes[size - 1U] ^= 1U; else ++size;
            Write(path, bytes, size); free(bytes);
        }
        const char *source = !strcmp(mode, "load-relative") ? "relative.umiv" :
            !strcmp(mode, "load-null") ? NULL : !strcmp(mode, "directory") ? root : path;
        CHECK(umi_open_tms_fx_forward_book_load(source, out) != UMI_STATUS_OK);
        CHECK(memcmp(out, before, sizeof *out) == 0);
    } else return 2;
    free(before); free(out); free(book);
    return 0;
}
