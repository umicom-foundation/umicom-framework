/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_trade.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/trade.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/trade.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradeContextTransferEqual(const UmiTradeContext *a, const UmiTradeContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->trade_id, b->trade_id) == 0 &&
        strcmp(a->source_system, b->source_system) == 0 &&
        strcmp(a->product_type, b->product_type) == 0 &&
        strcmp(a->book_id, b->book_id) == 0 &&
        strcmp(a->counterparty_id, b->counterparty_id) == 0 &&
        a->version == b->version &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradeContextTransferTails(UmiTradeContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->trade_id) + 1U;
        memset(value->trade_id + used, 0xa5, sizeof(value->trade_id) - used);
    }
    {
        size_t used = strlen(value->source_system) + 1U;
        memset(value->source_system + used, 0xa5, sizeof(value->source_system) - used);
    }
    {
        size_t used = strlen(value->product_type) + 1U;
        memset(value->product_type + used, 0xa5, sizeof(value->product_type) - used);
    }
    {
        size_t used = strlen(value->book_id) + 1U;
        memset(value->book_id + used, 0xa5, sizeof(value->book_id) - used);
    }
    {
        size_t used = strlen(value->counterparty_id) + 1U;
        memset(value->counterparty_id + used, 0xa5, sizeof(value->counterparty_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradeContextTransferMalformed(const UmiTradeContext *sample)
{
    (void)sample;
    {
        UmiTradeContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.trade_id, 'x', sizeof(invalid.trade_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_trade_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_trade_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated trade_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradeContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_system, 'x', sizeof(invalid.source_system));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_trade_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_trade_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_system was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradeContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.product_type, 'x', sizeof(invalid.product_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_trade_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_trade_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated product_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradeContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.book_id, 'x', sizeof(invalid.book_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_trade_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_trade_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated book_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradeContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.counterparty_id, 'x', sizeof(invalid.counterparty_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_trade_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_trade_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated counterparty_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradeContextTransferCases, UmiTradeContext,
    umi_trade_context_archive_encode, umi_trade_context_archive_decode,
    UmiTradeContextTransferEqual, UmiTradeContextTransferTails, UmiTradeContextTransferMalformed)

int main(void)
{
    UmiTradeContext value;
    umi_trade_context_init(&value);
    value.trade_id[0] = 's';
    value.source_system[0] = 's';
    value.product_type[0] = 's';
    value.book_id[0] = 's';
    value.counterparty_id[0] = 's';
    value.version = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_trade_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiTradeContextTransferCases(&value) != 0) return 1;

    return 0;
}
