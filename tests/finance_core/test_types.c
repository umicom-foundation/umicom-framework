/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_types.c
 *
 * PURPOSE:
 *   Exercise the types financial-core contract.
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
#include "umicom/finance/core/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

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
        if (!(!umi_financial_id_is_valid(&invalid)) ||
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

int main(void)
{
    UmiFinancialId id= {0}; CHECK(umi_financial_id_assign(&id,"T1")==UMI_STATUS_OK); CHECK(umi_financial_id_is_valid(&id));
    if (UmiFinancialIdTransferCases(&id) != 0) return 1;
 CHECK(umi_financial_date_is_valid((UmiFinancialDate){2028,2U,29U}));
    return 0;
}
