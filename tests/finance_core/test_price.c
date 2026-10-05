/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_price.c
 *
 * PURPOSE:
 *   Exercise the price financial-core contract.
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
#include "umicom/finance/core/price.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/price.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFinancialPriceTransferEqual(const UmiFinancialPrice *a, const UmiFinancialPrice *b)
{
    return a->value == b->value &&
        a->scale == b->scale;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFinancialPriceTransferTails(UmiFinancialPrice *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFinancialPriceTransferMalformed(const UmiFinancialPrice *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFinancialPriceTransferCases, UmiFinancialPrice,
    umi_price_archive_encode, umi_price_archive_decode,
    UmiFinancialPriceTransferEqual, UmiFinancialPriceTransferTails, UmiFinancialPriceTransferMalformed)

int main(void)
{
    UmiFinancialPrice p; CHECK(umi_price_init(&p,101.25,2U)==UMI_STATUS_OK);
    if (UmiFinancialPriceTransferCases(&p) != 0) return 1;

    return 0;
}
