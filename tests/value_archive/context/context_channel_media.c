/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_media.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/media.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/media.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiMediaContextTransferEqual(const UmiMediaContext *a, const UmiMediaContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->asset_id, b->asset_id) == 0 &&
        strcmp(a->timeline_id, b->timeline_id) == 0 &&
        strcmp(a->track_id, b->track_id) == 0 &&
        a->timecode_ms == b->timecode_ms &&
        a->duration_ms == b->duration_ms &&
        strcmp(a->media_type, b->media_type) == 0 &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiMediaContextTransferTails(UmiMediaContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->asset_id) + 1U;
        memset(value->asset_id + used, 0xa5, sizeof(value->asset_id) - used);
    }
    {
        size_t used = strlen(value->timeline_id) + 1U;
        memset(value->timeline_id + used, 0xa5, sizeof(value->timeline_id) - used);
    }
    {
        size_t used = strlen(value->track_id) + 1U;
        memset(value->track_id + used, 0xa5, sizeof(value->track_id) - used);
    }
    {
        size_t used = strlen(value->media_type) + 1U;
        memset(value->media_type + used, 0xa5, sizeof(value->media_type) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiMediaContextTransferMalformed(const UmiMediaContext *sample)
{
    (void)sample;
    {
        UmiMediaContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.asset_id, 'x', sizeof(invalid.asset_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_media_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_media_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated asset_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiMediaContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.timeline_id, 'x', sizeof(invalid.timeline_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_media_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_media_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated timeline_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiMediaContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.track_id, 'x', sizeof(invalid.track_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_media_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_media_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated track_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiMediaContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.media_type, 'x', sizeof(invalid.media_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_media_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_media_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated media_type was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiMediaContextTransferCases, UmiMediaContext,
    umi_media_context_archive_encode, umi_media_context_archive_decode,
    UmiMediaContextTransferEqual, UmiMediaContextTransferTails, UmiMediaContextTransferMalformed)

int main(void)
{
    UmiMediaContext value;
    umi_media_context_init(&value);
    value.asset_id[0] = 's';
    value.timeline_id[0] = 's';
    value.track_id[0] = 's';
    value.timecode_ms = (uint64_t)17U;
    value.duration_ms = (uint64_t)17U;
    value.media_type[0] = 's';
    value.revision = (uint64_t)17U;
    if (umi_media_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiMediaContextTransferCases(&value) != 0) return 1;

    return 0;
}
