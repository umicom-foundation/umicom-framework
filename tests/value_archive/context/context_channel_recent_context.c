/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_recent_context.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/recent_context.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/recent_context.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRecentContextTransferEqual(const UmiRecentContext *a, const UmiRecentContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->sequence == b->sequence &&
        a->last_used_ms == b->last_used_ms &&
        a->pinned == b->pinned &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRecentContextTransferTails(UmiRecentContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRecentContextTransferMalformed(const UmiRecentContext *sample)
{
    (void)sample;
    {
        UmiRecentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_recent_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_recent_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRecentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_recent_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_recent_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRecentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_recent_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_recent_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRecentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_recent_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_recent_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRecentContextTransferCases, UmiRecentContext,
    umi_context_recent_context_archive_encode, umi_context_recent_context_archive_decode,
    UmiRecentContextTransferEqual, UmiRecentContextTransferTails, UmiRecentContextTransferMalformed)

int main(void)
{
    UmiRecentContext value;
    umi_context_recent_context_init(&value);
    value.context_id[0] = 's';
    value.channel_id[0] = 's';
    value.schema_id[0] = 's';
    value.label[0] = 's';
    value.sequence = (uint64_t)17U;
    value.last_used_ms = (uint64_t)17U;
    value.pinned = true;
    value.revision = (uint64_t)17U;
    if (umi_context_recent_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiRecentContextTransferCases(&value) != 0) return 1;

    return 0;
}
