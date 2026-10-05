/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_loss_allowance.c
 *
 * PURPOSE:
 *   Implement the test loss allowance behavior for
 *   Umicom Framework.
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
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/loss_allowance.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/loss_allowance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityLossAllowanceTransferEqual(const UmiCommodityLossAllowance *a, const UmiCommodityLossAllowance *b)
{
    return strcmp(a->contract_id.value, b->contract_id.value) == 0 &&
        a->basis_points == b->basis_points &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityLossAllowanceTransferTails(UmiCommodityLossAllowance *value)
{
    (void)value;
    {
        size_t used = strlen(value->contract_id.value) + 1U;
        memset(value->contract_id.value + used, 0xa5, sizeof(value->contract_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityLossAllowanceTransferMalformed(const UmiCommodityLossAllowance *sample)
{
    (void)sample;
    {
        UmiCommodityLossAllowance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.contract_id.value, 'x', sizeof(invalid.contract_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_loss_allowance_valid(&invalid)) ||
            umi_commodity_loss_allowance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated contract_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityLossAllowanceTransferCases, UmiCommodityLossAllowance,
    umi_commodity_loss_allowance_archive_encode, umi_commodity_loss_allowance_archive_decode,
    UmiCommodityLossAllowanceTransferEqual, UmiCommodityLossAllowanceTransferTails, UmiCommodityLossAllowanceTransferMalformed)

int main(void)
{
    UmiCommodityLossAllowance value;
    CHECK(umi_commodity_loss_allowance_init(&value, "CTR-1", 25) == UMI_STATUS_OK);
    CHECK(umi_commodity_loss_allowance_valid(&value));
    if (UmiCommodityLossAllowanceTransferCases(&value) != 0) return 1;

    return 0;
}
