/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_nomination.c
 *
 * PURPOSE:
 *   Implement the test nomination behavior for
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

#include "umicom/finance/commodity/nomination.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/nomination.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityNominationTransferEqual(const UmiCommodityNomination *a, const UmiCommodityNomination *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->contract_id.value, b->contract_id.value) == 0 &&
        a->quantity.units == b->quantity.units &&
        a->quantity.scale == b->quantity.scale &&
        strcmp(a->quantity.unit_code, b->quantity.unit_code) == 0 &&
        a->window_start_ms == b->window_start_ms &&
        a->window_end_ms == b->window_end_ms &&
        a->accepted == b->accepted;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityNominationTransferTails(UmiCommodityNomination *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->contract_id.value) + 1U;
        memset(value->contract_id.value + used, 0xa5, sizeof(value->contract_id.value) - used);
    }
    {
        size_t used = strlen(value->quantity.unit_code) + 1U;
        memset(value->quantity.unit_code + used, 0xa5, sizeof(value->quantity.unit_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityNominationTransferMalformed(const UmiCommodityNomination *sample)
{
    (void)sample;
    {
        UmiCommodityNomination invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_nomination_valid(&invalid)) ||
            umi_commodity_nomination_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityNomination invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.contract_id.value, 'x', sizeof(invalid.contract_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_nomination_valid(&invalid)) ||
            umi_commodity_nomination_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated contract_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityNomination invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.quantity.unit_code, 'x', sizeof(invalid.quantity.unit_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_nomination_valid(&invalid)) ||
            umi_commodity_nomination_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated quantity.unit_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityNominationTransferCases, UmiCommodityNomination,
    umi_commodity_nomination_archive_encode, umi_commodity_nomination_archive_decode,
    UmiCommodityNominationTransferEqual, UmiCommodityNominationTransferTails, UmiCommodityNominationTransferMalformed)

int main(void)
{
    UmiCommodityNomination value;
    CHECK(umi_commodity_nomination_init(&value, "NOM-1", "CTR-1", 250, 0, "MT", 1000, 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_nomination_valid(&value));
    if (UmiCommodityNominationTransferCases(&value) != 0) return 1;

    return 0;
}
