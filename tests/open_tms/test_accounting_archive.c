/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_archive.c
 *
 * PURPOSE:
 *   Check accounting archive admission, per-currency approval and atomic replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_archive.h"
#include "../../src/base/value_archive_internal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static void Pair(UmiOpenTmsAccountingBook *book, const char *currency, double amount) {
    UmiOpenTmsAccountingLine row = {0};
    strcpy(row.accountId, "cash"); strcpy(row.referenceId, "sample");
    memcpy(row.currency, currency, 4U); row.debit = amount;
    CHECK(umi_open_tms_accounting_book_add(book, &row) == UMI_STATUS_OK);
    strcpy(row.accountId, "receivable"); row.debit = 0.0; row.credit = amount;
    CHECK(umi_open_tms_accounting_book_add(book, &row) == UMI_STATUS_OK);
}
static size_t Numeric(const unsigned char *bytes, size_t row) {
    for (unsigned field = 0U; field < 3U; ++field)
        row += 8U + (size_t)UmiArchiveLoadInteger(bytes + row, 8U);
    return row;
}
static void Reseal(unsigned char *bytes, size_t count) {
    UmiArchiveStoreInteger(bytes + 24U, UmiArchiveChecksum(bytes, count), 4U);
}
static void Same(const void *a, const void *b, size_t count) { CHECK(memcmp(a, b, count) == 0); }
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book), *out = malloc(sizeof *out);
    UmiOpenTmsAccountingBook *scratch = malloc(sizeof *scratch), *before = malloc(sizeof *before);
    CHECK(book && out && scratch && before);
    umi_open_tms_accounting_book_init(book);
    if (strcmp(mode, "empty")) Pair(book, "GBP", 125.25);
    if (!strcmp(mode, "multi")) Pair(book, "USD", 2.5);
    if (!strcmp(mode, "full"))
        while (book->count < UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) Pair(book, "GBP", 0.25);
    if (!strcmp(mode, "draft-unbalanced")) book->lines[1].credit = 9.0;
    if (!strcmp(mode, "approved") || !strcmp(mode, "posted")) {
        CHECK(umi_open_tms_accounting_book_approve(book) == UMI_STATUS_OK);
        if (!strcmp(mode, "posted")) CHECK(umi_open_tms_accounting_book_post(book) == UMI_STATUS_OK);
    }
    if (!strcmp(mode, "maximum-revision")) book->revision = UINT64_MAX;
    size_t length = 0U;
    CHECK(umi_open_tms_accounting_book_archive_encode(book, NULL, 0U, &length) == UMI_STATUS_OK);
    CHECK(length <= umi_open_tms_accounting_book_archive_limit());
    unsigned char *bytes = malloc(length + 1U), *copy = malloc(length + 1U);
    CHECK(bytes && copy);
    memset(bytes, 0xAA, length + 1U);
    CHECK(umi_open_tms_accounting_book_archive_encode(book, bytes, length, &length) == UMI_STATUS_OK);
    memcpy(copy, bytes, length + 1U);
    memset(out, 0xA5, sizeof *out); memcpy(before, out, sizeof *out);
    if (!strcmp(mode, "roundtrip") || !strcmp(mode, "empty") || !strcmp(mode, "full") ||
        !strcmp(mode, "multi") || !strcmp(mode, "approved") || !strcmp(mode, "posted") ||
        !strcmp(mode, "maximum-revision") || !strcmp(mode, "draft-unbalanced") || !strcmp(mode, "workspace")) {
        UmiStatus status = !strcmp(mode, "workspace")
            ? umi_open_tms_accounting_book_archive_decode_with_workspace(bytes, length, scratch, out)
            : umi_open_tms_accounting_book_archive_decode(bytes, length, out);
        CHECK(status == UMI_STATUS_OK && out->count == book->count && out->revision == book->revision);
        CHECK(out->approved == book->approved && out->posted == book->posted);
        CHECK(umi_open_tms_accounting_book_validate(out) == UMI_STATUS_OK);
        size_t written = 0U;
        CHECK(umi_open_tms_accounting_book_archive_encode(out, copy, length, &written) == UMI_STATUS_OK);
        CHECK(written == length); Same(bytes, copy, length);
        if (!strcmp(mode, "draft-unbalanced"))
            CHECK(umi_open_tms_accounting_book_check_balance(out) == UMI_STATUS_INVALID_STATE);
        else if (out->count != 0U) CHECK(umi_open_tms_accounting_book_check_balance(out) == UMI_STATUS_OK);
    } else if (!strcmp(mode, "short")) {
        memset(copy, 0xA5, length + 1U); size_t needed = 0U;
        CHECK(umi_open_tms_accounting_book_archive_encode(book, copy, length - 1U, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(needed == length);
        for (size_t i = 0U; i <= length; ++i) CHECK(copy[i] == 0xA5U);
    } else if (!strcmp(mode, "tails")) {
        book->lines[0].accountId[63] = 'x';
        memset(&book->lines[2], 0xCC, sizeof book->lines[2]);
        size_t written = 0U;
        CHECK(umi_open_tms_accounting_book_archive_encode(book, copy, length, &written) == UMI_STATUS_OK);
        CHECK(written == length); Same(bytes, copy, length);
        CHECK(umi_open_tms_accounting_book_archive_decode(bytes, length, out) == UMI_STATUS_OK);
        CHECK(out->lines[0].accountId[63] == '\0' && out->lines[2].accountId[0] == '\0');
    } else if (!strcmp(mode, "truncation")) {
        for (size_t count = 0U; count < length; ++count) {
            CHECK(umi_open_tms_accounting_book_archive_decode(bytes, count, out) != UMI_STATUS_OK);
            Same(out, before, sizeof *out);
        }
    } else if (!strncmp(mode, "conditional", 11U)) {
        umi_open_tms_accounting_book_init(out); Pair(out, "USD", 5.0); *before = *out;
        if (!strcmp(mode, "conditional-stale")) ++out->revision;
        else if (!strcmp(mode, "conditional-row")) {
            out->lines[0].debit = 6.0; out->lines[1].credit = 6.0;
        } else if (!strcmp(mode, "conditional-tail")) out->lines[0].accountId[63] = 'x';
        else if (!strcmp(mode, "conditional-invalid")) bytes[32] ^= 1U;
        else CHECK(!strcmp(mode, "conditional"));
        *scratch = *out;
        UmiStatus status = umi_open_tms_accounting_book_archive_decode_if_unchanged(bytes, length, before, out);
        if (!strcmp(mode, "conditional") || !strcmp(mode, "conditional-tail")) {
            CHECK(status == UMI_STATUS_OK && out->count == 2U);
            CHECK(!strcmp(out->lines[0].currency, "GBP") && out->lines[0].debit == 125.25);
        } else { CHECK(status != UMI_STATUS_OK); Same(out, scratch, sizeof *out); }
    } else if (!strcmp(mode, "overlap")) {
        CHECK(umi_open_tms_accounting_book_archive_decode_with_workspace(bytes, length, out, out) == UMI_STATUS_INVALID_ARGUMENT);
        Same(out, before, sizeof *out);
        CHECK(length < sizeof *scratch); memcpy(scratch, bytes, length);
        CHECK(umi_open_tms_accounting_book_archive_decode_with_workspace(scratch, length, scratch, out) == UMI_STATUS_INVALID_ARGUMENT);
        Same(out, before, sizeof *out);
    } else if (!strcmp(mode, "invalid-api")) {
        size_t count = 55U;
        CHECK(umi_open_tms_accounting_book_check_balance(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_accounting_book_archive_encode(NULL, copy, length, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(count == 55U); Same(copy, bytes, length);
        CHECK(umi_open_tms_accounting_book_archive_encode(book, NULL, 1U, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(count == 55U);
        CHECK(umi_open_tms_accounting_book_archive_decode(NULL, length, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_accounting_book_archive_decode_with_workspace(bytes, length, NULL, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_accounting_book_archive_decode_if_unchanged(bytes, length, NULL, out) == UMI_STATUS_INVALID_ARGUMENT);
        Same(out, before, sizeof *out);
    } else if (!strncmp(mode, "encode-", 7U)) {
        if (!strcmp(mode, "encode-count")) book->count = UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY + 1U;
        else if (!strcmp(mode, "encode-text")) memset(book->lines[0].accountId, 'x', sizeof book->lines[0].accountId);
        else if (!strcmp(mode, "encode-nan")) book->lines[0].debit = NAN;
        else if (!strcmp(mode, "encode-approved")) { book->approved = 1; book->lines[1].credit = 2.0; }
        else if (!strcmp(mode, "encode-posted")) book->posted = 1;
        else CHECK(0);
        size_t count = 55U;
        CHECK(umi_open_tms_accounting_book_archive_encode(book, copy, length, &count) != UMI_STATUS_OK);
        CHECK(count == 55U); Same(bytes, copy, length);
    } else {
        /* The envelope is resealed for semantic corruption cases so admission
         * must inspect decoded fields, rather than relying on its checksum. */
        size_t first = 64U, numeric = Numeric(bytes, first), count = length;
        if (!strcmp(mode, "checksum")) bytes[32] ^= 1U;
        else if (!strcmp(mode, "schema")) { bytes[8] ^= 1U; Reseal(bytes, length); }
        else if (!strcmp(mode, "trailing")) { bytes[length] = 1U; ++count; }
        else {
            if (!strcmp(mode, "count")) UmiArchiveStoreInteger(bytes + 40U, UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY + 1U, 8U);
            else if (!strcmp(mode, "revision")) UmiArchiveStoreInteger(bytes + 32U, 0U, 8U);
            else if (!strcmp(mode, "approved-flag")) UmiArchiveStoreInteger(bytes + 48U, 2U, 8U);
            else if (!strcmp(mode, "posted-flag")) UmiArchiveStoreInteger(bytes + 56U, 2U, 8U);
            else if (!strcmp(mode, "posted-without-approval")) UmiArchiveStoreInteger(bytes + 56U, 1U, 8U);
            else if (!strcmp(mode, "approved-unbalanced") || !strcmp(mode, "approved-cross-currency")) {
                UmiArchiveStoreInteger(bytes + 48U, 1U, 8U);
                if (!strcmp(mode, "approved-unbalanced"))
                    UmiArchiveStoreInteger(bytes + numeric, UINT64_C(0x3ff0000000000000), 8U);
                else {
                    size_t second = numeric + 16U;
                    size_t currency = second + 8U + (size_t)UmiArchiveLoadInteger(bytes + second, 8U) + 8U;
                    memcpy(bytes + currency, "USD", 3U);
                }
            } else if (!strcmp(mode, "text-length")) UmiArchiveStoreInteger(bytes + first, UINT64_MAX, 8U);
            else if (!strcmp(mode, "text-nul")) bytes[first + 8U] = 0U;
            else if (!strcmp(mode, "text-control")) bytes[first + 8U] = 1U;
            else if (!strcmp(mode, "nan")) UmiArchiveStoreInteger(bytes + numeric, UINT64_C(0x7ff8000000000000), 8U);
            else if (!strcmp(mode, "negative")) UmiArchiveStoreInteger(bytes + numeric, UINT64_C(0xbff0000000000000), 8U);
            else if (!strcmp(mode, "both-sides")) UmiArchiveStoreInteger(bytes + numeric + 8U, UINT64_C(0x3ff0000000000000), 8U);
            else CHECK(0);
            Reseal(bytes, length);
        }
        CHECK(umi_open_tms_accounting_book_archive_decode(bytes, count, out) != UMI_STATUS_OK);
        Same(out, before, sizeof *out);
        CHECK(umi_open_tms_accounting_book_archive_decode_with_workspace(bytes, count, scratch, out) != UMI_STATUS_OK);
        Same(out, before, sizeof *out);
    }
    free(copy); free(bytes); free(before); free(scratch); free(out); free(book);
    return 0;
}
