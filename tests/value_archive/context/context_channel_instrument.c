/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_instrument.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/instrument.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/instrument.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiInstrumentContextTransferEqual(const UmiInstrumentContext *a, const UmiInstrumentContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->instrument_id, b->instrument_id) == 0 &&
        strcmp(a->symbol, b->symbol) == 0 &&
        strcmp(a->venue, b->venue) == 0 &&
        strcmp(a->currency, b->currency) == 0 &&
        strcmp(a->asset_class, b->asset_class) == 0 &&
        strcmp(a->contract_id, b->contract_id) == 0 &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiInstrumentContextTransferTails(UmiInstrumentContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->instrument_id) + 1U;
        memset(value->instrument_id + used, 0xa5, sizeof(value->instrument_id) - used);
    }
    {
        size_t used = strlen(value->symbol) + 1U;
        memset(value->symbol + used, 0xa5, sizeof(value->symbol) - used);
    }
    {
        size_t used = strlen(value->venue) + 1U;
        memset(value->venue + used, 0xa5, sizeof(value->venue) - used);
    }
    {
        size_t used = strlen(value->currency) + 1U;
        memset(value->currency + used, 0xa5, sizeof(value->currency) - used);
    }
    {
        size_t used = strlen(value->asset_class) + 1U;
        memset(value->asset_class + used, 0xa5, sizeof(value->asset_class) - used);
    }
    {
        size_t used = strlen(value->contract_id) + 1U;
        memset(value->contract_id + used, 0xa5, sizeof(value->contract_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiInstrumentContextTransferMalformed(const UmiInstrumentContext *sample)
{
    (void)sample;
    {
        UmiInstrumentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument_id, 'x', sizeof(invalid.instrument_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_instrument_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_instrument_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstrumentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.symbol, 'x', sizeof(invalid.symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_instrument_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_instrument_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated symbol was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstrumentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.venue, 'x', sizeof(invalid.venue));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_instrument_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_instrument_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated venue was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstrumentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.currency, 'x', sizeof(invalid.currency));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_instrument_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_instrument_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated currency was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstrumentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.asset_class, 'x', sizeof(invalid.asset_class));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_instrument_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_instrument_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated asset_class was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstrumentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.contract_id, 'x', sizeof(invalid.contract_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_instrument_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_instrument_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated contract_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiInstrumentContextTransferCases, UmiInstrumentContext,
    umi_instrument_context_archive_encode, umi_instrument_context_archive_decode,
    UmiInstrumentContextTransferEqual, UmiInstrumentContextTransferTails, UmiInstrumentContextTransferMalformed)

int main(void)
{
    UmiInstrumentContext value;
    umi_instrument_context_init(&value);
    value.instrument_id[0] = 's';
    value.symbol[0] = 's';
    value.venue[0] = 's';
    value.currency[0] = 's';
    value.asset_class[0] = 's';
    value.contract_id[0] = 's';
    value.revision = (uint64_t)17U;
    if (umi_instrument_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiInstrumentContextTransferCases(&value) != 0) return 1;

    return 0;
}
