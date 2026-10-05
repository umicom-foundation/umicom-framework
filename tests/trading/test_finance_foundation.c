/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_finance_foundation.c
 *
 * PURPOSE:
 *   Validate finance foundation behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "umicom/finance/finance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCurrencyTransferEqual(const UmiCurrency *a, const UmiCurrency *b)
{
    return strcmp(a->code, b->code) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCurrencyTransferTails(UmiCurrency *value)
{
    (void)value;
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCurrencyTransferMalformed(const UmiCurrency *sample)
{
    (void)sample;
    {
        UmiCurrency invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.code, 'x', sizeof(invalid.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_currency_valid(&invalid)) ||
            umi_currency_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCurrencyTransferCases, UmiCurrency,
    umi_currency_archive_encode, umi_currency_archive_decode,
    UmiCurrencyTransferEqual, UmiCurrencyTransferTails, UmiCurrencyTransferMalformed)

#include "umicom/finance/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFinancialIdTransferEqual(const UmiFinancialId *a, const UmiFinancialId *b)
{
    return strcmp(a->value, b->value) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFinancialIdTransferTails(UmiFinancialId *value)
{
    (void)value;
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFinancialIdTransferMalformed(const UmiFinancialId *sample)
{
    (void)sample;
    {
        UmiFinancialId invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value, 'x', sizeof(invalid.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_financial_id_valid(&invalid)) ||
            umi_financial_id_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFinancialIdTransferCases, UmiFinancialId,
    umi_financial_id_archive_encode, umi_financial_id_archive_decode,
    UmiFinancialIdTransferEqual, UmiFinancialIdTransferTails, UmiFinancialIdTransferMalformed)

int main(void){
    UmiFinancialId id={0};(void)snprintf(id.value,sizeof(id.value),"%s","A-1");assert(umi_financial_id_valid(&id));
    if (UmiFinancialIdTransferCases(&id) != 0) return 1;

    UmiCurrency usd={{'U','S','D','\0'}};assert(umi_currency_valid(&usd));
    if (UmiCurrencyTransferCases(&usd) != 0) return 1;

    UmiMoney a={1000,2,{{'U','S','D','\0'}}},b={250,2,{{'U','S','D','\0'}}},out={0};
    assert(umi_money_add(&a,&b,&out)==UMI_STATUS_OK);assert(out.minor_units==1250);
    UmiDecimal d={123,2},r={0};assert(umi_decimal_rescale(d,4U,&r)==UMI_STATUS_OK);assert(r.coefficient==12300);
    return 0;
}
