/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_book.c
 *
 * PURPOSE:
 *   Exercise the book financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <string.h>
#include "umicom/finance/core/book.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/book.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFinancialBookTransferEqual(const UmiFinancialBook *a, const UmiFinancialBook *b)
{
    return strcmp(a->book_id.value, b->book_id.value) == 0 &&
        strcmp(a->parent_id.value, b->parent_id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFinancialBookTransferTails(UmiFinancialBook *value)
{
    (void)value;
    {
        size_t used = strlen(value->book_id.value) + 1U;
        memset(value->book_id.value + used, 0xa5, sizeof(value->book_id.value) - used);
    }
    {
        size_t used = strlen(value->parent_id.value) + 1U;
        memset(value->parent_id.value + used, 0xa5, sizeof(value->parent_id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFinancialBookTransferMalformed(const UmiFinancialBook *sample)
{
    (void)sample;
    {
        UmiFinancialBook invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.book_id.value, 'x', sizeof(invalid.book_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_book_is_valid(&invalid)) ||
            umi_book_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated book_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFinancialBook invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id.value, 'x', sizeof(invalid.parent_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_book_is_valid(&invalid)) ||
            umi_book_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFinancialBook invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_book_is_valid(&invalid)) ||
            umi_book_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFinancialBookTransferCases, UmiFinancialBook,
    umi_book_archive_encode, umi_book_archive_decode,
    UmiFinancialBookTransferEqual, UmiFinancialBookTransferTails, UmiFinancialBookTransferMalformed)

int main(void)
{
    UmiFinancialBook x; CHECK(umi_book_init(&x,"ID","Name","PARENT")==UMI_STATUS_OK); CHECK(umi_book_is_valid(&x));
    if (UmiFinancialBookTransferCases(&x) != 0) return 1;

    return 0;
}
