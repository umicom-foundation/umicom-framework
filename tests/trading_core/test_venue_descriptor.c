/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_descriptor.c
 *
 * PURPOSE:
 *   Exercise define exchange and execution-venue identity and capabilities.
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
#include "umicom/trading/core/venue_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/venue_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingVenueDescriptorTransferEqual(const UmiTradingVenueDescriptor *a, const UmiTradingVenueDescriptor *b)
{
    return strcmp(a->venue_id.value, b->venue_id.value) == 0 &&
        strcmp(a->mic, b->mic) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->supports_auctions == b->supports_auctions &&
        a->supports_hidden_liquidity == b->supports_hidden_liquidity &&
        a->priority == b->priority;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingVenueDescriptorTransferTails(UmiTradingVenueDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->venue_id.value) + 1U;
        memset(value->venue_id.value + used, 0xa5, sizeof(value->venue_id.value) - used);
    }
    {
        size_t used = strlen(value->mic) + 1U;
        memset(value->mic + used, 0xa5, sizeof(value->mic) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingVenueDescriptorTransferMalformed(const UmiTradingVenueDescriptor *sample)
{
    (void)sample;
    {
        UmiTradingVenueDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.venue_id.value, 'x', sizeof(invalid.venue_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_venue_descriptor_valid(&invalid)) ||
            umi_trading_venue_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated venue_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradingVenueDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.mic, 'x', sizeof(invalid.mic));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_venue_descriptor_valid(&invalid)) ||
            umi_trading_venue_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated mic was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradingVenueDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_venue_descriptor_valid(&invalid)) ||
            umi_trading_venue_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingVenueDescriptorTransferCases, UmiTradingVenueDescriptor,
    umi_trading_venue_descriptor_archive_encode, umi_trading_venue_descriptor_archive_decode,
    UmiTradingVenueDescriptorTransferEqual, UmiTradingVenueDescriptorTransferTails, UmiTradingVenueDescriptorTransferMalformed)

int main(void) {

    UmiTradingVenueDescriptor v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_venue_descriptor_init(&v,"venue-lse","XLON","London",true,false,10U)!=UMI_STATUS_OK)return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_trading_venue_descriptor_valid(&v)||!v.supports_auctions)return 2;
    if (UmiTradingVenueDescriptorTransferCases(&v) != 0) return 1;

    return 0;
}
