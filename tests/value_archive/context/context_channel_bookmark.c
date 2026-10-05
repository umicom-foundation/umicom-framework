/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_bookmark.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/bookmark.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/bookmark.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextBookmarkTransferEqual(const UmiContextBookmark *a, const UmiContextBookmark *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->bookmark_id, b->bookmark_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->user_id, b->user_id) == 0 &&
        a->shared == b->shared &&
        a->created_at_ms == b->created_at_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextBookmarkTransferTails(UmiContextBookmark *value)
{
    (void)value;
    {
        size_t used = strlen(value->bookmark_id) + 1U;
        memset(value->bookmark_id + used, 0xa5, sizeof(value->bookmark_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->user_id) + 1U;
        memset(value->user_id + used, 0xa5, sizeof(value->user_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextBookmarkTransferMalformed(const UmiContextBookmark *sample)
{
    (void)sample;
    {
        UmiContextBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bookmark_id, 'x', sizeof(invalid.bookmark_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bookmark_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.user_id, 'x', sizeof(invalid.user_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated user_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextBookmarkTransferCases, UmiContextBookmark,
    umi_context_bookmark_archive_encode, umi_context_bookmark_archive_decode,
    UmiContextBookmarkTransferEqual, UmiContextBookmarkTransferTails, UmiContextBookmarkTransferMalformed)

int main(void)
{
    UmiContextBookmark value;
    umi_context_bookmark_init(&value);
    value.bookmark_id[0] = 's';
    value.context_id[0] = 's';
    value.channel_id[0] = 's';
    value.label[0] = 's';
    value.user_id[0] = 's';
    value.shared = true;
    value.created_at_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_bookmark_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextBookmarkTransferCases(&value) != 0) return 1;

    return 0;
}
