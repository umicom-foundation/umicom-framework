/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_factor.c
 *
 * PURPOSE:
 *   Exercise risk factor validation and calculations.
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
#include "umicom/finance/treasury/risk_factor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/treasury/risk_factor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTreasuryRiskFactorTransferEqual(const UmiTreasuryRiskFactor *a, const UmiTreasuryRiskFactor *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->risk_class == b->risk_class &&
        a->shock_bps == b->shock_bps;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTreasuryRiskFactorTransferTails(UmiTreasuryRiskFactor *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTreasuryRiskFactorTransferMalformed(const UmiTreasuryRiskFactor *sample)
{
    (void)sample;
    {
        UmiTreasuryRiskFactor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_treasury_risk_factor_valid(&invalid)) ||
            umi_treasury_risk_factor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTreasuryRiskFactorTransferCases, UmiTreasuryRiskFactor,
    umi_treasury_risk_factor_archive_encode, umi_treasury_risk_factor_archive_decode,
    UmiTreasuryRiskFactorTransferEqual, UmiTreasuryRiskFactorTransferTails, UmiTreasuryRiskFactorTransferMalformed)

int main(void) {
    UmiTreasuryRiskFactor v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_factor_init(&v, "USD-IR", UMI_TREASURY_RISK_MARKET, -25) != UMI_STATUS_OK) return 1;
    if (UmiTreasuryRiskFactorTransferCases(&v) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_risk_factor_absolute_shock_bps(&v)!=25)return 2;
    return 0;
}
