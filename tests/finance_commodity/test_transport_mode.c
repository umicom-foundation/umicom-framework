/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_transport_mode.c
 *
 * PURPOSE:
 *   Implement the test transport mode behavior for
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

#include "umicom/finance/commodity/transport_mode.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/transport_mode.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityTransportModeTransferEqual(const UmiCommodityTransportMode *a, const UmiCommodityTransportMode *b)
{
    return strcmp(a->code, b->code) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->supports_bulk == b->supports_bulk &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityTransportModeTransferTails(UmiCommodityTransportMode *value)
{
    (void)value;
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityTransportModeTransferMalformed(const UmiCommodityTransportMode *sample)
{
    (void)sample;
    {
        UmiCommodityTransportMode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.code, 'x', sizeof(invalid.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_mode_valid(&invalid)) ||
            umi_commodity_transport_mode_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated code was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityTransportMode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_mode_valid(&invalid)) ||
            umi_commodity_transport_mode_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityTransportModeTransferCases, UmiCommodityTransportMode,
    umi_commodity_transport_mode_archive_encode, umi_commodity_transport_mode_archive_decode,
    UmiCommodityTransportModeTransferEqual, UmiCommodityTransportModeTransferTails, UmiCommodityTransportModeTransferMalformed)

int main(void)
{
    UmiCommodityTransportMode value;
    CHECK(umi_commodity_transport_mode_init(&value, "VESSEL", "Ocean vessel", true) == UMI_STATUS_OK);
    CHECK(umi_commodity_transport_mode_valid(&value));
    if (UmiCommodityTransportModeTransferCases(&value) != 0) return 1;

    return 0;
}
