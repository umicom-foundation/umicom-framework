/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_channel.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/channel.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/channel.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextChannelTransferEqual(const UmiContextChannel *a, const UmiContextChannel *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        a->kind == b->kind &&
        a->colour == b->colour &&
        a->enabled == b->enabled &&
        a->persistent == b->persistent &&
        a->last_sequence == b->last_sequence &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextChannelTransferTails(UmiContextChannel *value)
{
    (void)value;
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextChannelTransferMalformed(const UmiContextChannel *sample)
{
    (void)sample;
    {
        UmiContextChannel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_channel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_channel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextChannel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_channel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_channel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextChannel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_channel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_channel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextChannelTransferCases, UmiContextChannel,
    umi_context_channel_archive_encode, umi_context_channel_archive_decode,
    UmiContextChannelTransferEqual, UmiContextChannelTransferTails, UmiContextChannelTransferMalformed)

int main(void)
{
    UmiContextChannel value;
    umi_context_channel_init(&value);
    value.channel_id[0] = 's';
    value.display_name[0] = 's';
    value.schema_id[0] = 's';
    value.enabled = true;
    value.persistent = true;
    value.last_sequence = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_channel_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextChannelTransferCases(&value) != 0) return 1;

    return 0;
}
