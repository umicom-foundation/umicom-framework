/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_market_data_key.c
 *
 * PURPOSE:
 *   Exercise the market data key financial-core contract.
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
#include "umicom/finance/core/market_data_key.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/market_data_key.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiMarketDataKeyTransferEqual(const UmiMarketDataKey *a, const UmiMarketDataKey *b)
{
    return strcmp(a->key_id.value, b->key_id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->code, b->code) == 0 &&
        a->state == b->state &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiMarketDataKeyTransferTails(UmiMarketDataKey *value)
{
    (void)value;
    {
        size_t used = strlen(value->key_id.value) + 1U;
        memset(value->key_id.value + used, 0xa5, sizeof(value->key_id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiMarketDataKeyTransferMalformed(const UmiMarketDataKey *sample)
{
    (void)sample;
    {
        UmiMarketDataKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key_id.value, 'x', sizeof(invalid.key_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_market_data_key_is_valid(&invalid)) ||
            umi_market_data_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiMarketDataKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_market_data_key_is_valid(&invalid)) ||
            umi_market_data_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiMarketDataKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.code, 'x', sizeof(invalid.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_market_data_key_is_valid(&invalid)) ||
            umi_market_data_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiMarketDataKeyTransferCases, UmiMarketDataKey,
    umi_market_data_key_archive_encode, umi_market_data_key_archive_decode,
    UmiMarketDataKeyTransferEqual, UmiMarketDataKeyTransferTails, UmiMarketDataKeyTransferMalformed)

int main(void)
{
    UmiMarketDataKey x; CHECK(umi_market_data_key_init(&x,"ID","Name","CODE",1U)==UMI_STATUS_OK); CHECK(umi_market_data_key_is_valid(&x));
    if (UmiMarketDataKeyTransferCases(&x) != 0) return 1;

    return 0;
}
