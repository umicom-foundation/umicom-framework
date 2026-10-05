/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_trade_id.c
 *
 * PURPOSE:
 *   Exercise the trade id financial-core contract.
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
#include "umicom/finance/core/trade_id.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/trade_id.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradeIdTransferEqual(const UmiTradeId *a, const UmiTradeId *b)
{
    return strcmp(a->id.value, b->id.value) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradeIdTransferTails(UmiTradeId *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradeIdTransferMalformed(const UmiTradeId *sample)
{
    (void)sample;
    {
        UmiTradeId invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trade_id_is_valid(&invalid)) ||
            umi_trade_id_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradeIdTransferCases, UmiTradeId,
    umi_trade_id_archive_encode, umi_trade_id_archive_decode,
    UmiTradeIdTransferEqual, UmiTradeIdTransferTails, UmiTradeIdTransferMalformed)

int main(void)
{
    UmiTradeId x= {0}; CHECK(umi_trade_id_set(&x,"ID")==UMI_STATUS_OK); CHECK(umi_trade_id_is_valid(&x));
    if (UmiTradeIdTransferCases(&x) != 0) return 1;

    return 0;
}
