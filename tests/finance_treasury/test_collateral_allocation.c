/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_collateral_allocation.c
 *
 * PURPOSE:
 *   Exercise collateral allocation validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/finance/treasury/collateral_allocation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/treasury/collateral_allocation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTreasuryCollateralAllocationTransferEqual(const UmiTreasuryCollateralAllocation *a, const UmiTreasuryCollateralAllocation *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->required_minor == b->required_minor &&
        a->allocated_minor == b->allocated_minor;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTreasuryCollateralAllocationTransferTails(UmiTreasuryCollateralAllocation *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTreasuryCollateralAllocationTransferMalformed(const UmiTreasuryCollateralAllocation *sample)
{
    (void)sample;
    {
        UmiTreasuryCollateralAllocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_treasury_collateral_allocation_valid(&invalid)) ||
            umi_treasury_collateral_allocation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTreasuryCollateralAllocationTransferCases, UmiTreasuryCollateralAllocation,
    umi_treasury_collateral_allocation_archive_encode, umi_treasury_collateral_allocation_archive_decode,
    UmiTreasuryCollateralAllocationTransferEqual, UmiTreasuryCollateralAllocationTransferTails, UmiTreasuryCollateralAllocationTransferMalformed)

int main(void) {
    UmiTreasuryCollateralAllocation v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_collateral_allocation_init(&v, "alloc", 1000, 850) != UMI_STATUS_OK) return 1;
    if (UmiTreasuryCollateralAllocationTransferCases(&v) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_collateral_allocation_shortfall_minor(&v)!=150)return 2;
    return 0;
}
