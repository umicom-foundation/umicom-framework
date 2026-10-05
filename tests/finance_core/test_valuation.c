/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_valuation.c
 *
 * PURPOSE:
 *   Exercise the valuation financial-core contract.
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
#include "umicom/finance/core/valuation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/valuation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiValuationTransferEqual(const UmiValuation *a, const UmiValuation *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        a->amount.minor_units == b->amount.minor_units &&
        a->amount.scale == b->amount.scale &&
        strcmp(a->amount.currency.code, b->amount.currency.code) == 0 &&
        a->date.year == b->date.year &&
        a->date.month == b->date.month &&
        a->date.day == b->date.day &&
        a->state == b->state;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiValuationTransferTails(UmiValuation *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->amount.currency.code) + 1U;
        memset(value->amount.currency.code + used, 0xa5, sizeof(value->amount.currency.code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiValuationTransferMalformed(const UmiValuation *sample)
{
    (void)sample;
    {
        UmiValuation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_valuation_is_valid(&invalid)) ||
            umi_valuation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiValuation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.amount.currency.code, 'x', sizeof(invalid.amount.currency.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_valuation_is_valid(&invalid)) ||
            umi_valuation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated amount.currency.code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiValuationTransferCases, UmiValuation,
    umi_valuation_archive_encode, umi_valuation_archive_decode,
    UmiValuationTransferEqual, UmiValuationTransferTails, UmiValuationTransferMalformed)

int main(void)
{
    UmiValuation x; UmiMoney m={10,2U,{{'U','S','D','\0'}}}; CHECK(umi_valuation_init(&x,"ID",m,(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_valuation_is_valid(&x));
    if (UmiValuationTransferCases(&x) != 0) return 1;

    return 0;
}
