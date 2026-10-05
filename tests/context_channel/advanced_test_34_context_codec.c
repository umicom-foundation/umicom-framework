/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_34_context_codec.c
 *
 * PURPOSE:
 *   Validate context codec sequence accounting, bounded fields and failure evidence.
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
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_codec.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_codec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextCodecTransferEqual(const UmiContextCodec *a, const UmiContextCodec *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->codec_id, b->codec_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->media_type, b->media_type) == 0 &&
        strcmp(a->encoding, b->encoding) == 0 &&
        a->first_sequence == b->first_sequence &&
        a->last_sequence == b->last_sequence &&
        a->item_count == b->item_count &&
        a->failure_count == b->failure_count &&
        a->status == b->status &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextCodecTransferTails(UmiContextCodec *value)
{
    (void)value;
    {
        size_t used = strlen(value->codec_id) + 1U;
        memset(value->codec_id + used, 0xa5, sizeof(value->codec_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->media_type) + 1U;
        memset(value->media_type + used, 0xa5, sizeof(value->media_type) - used);
    }
    {
        size_t used = strlen(value->encoding) + 1U;
        memset(value->encoding + used, 0xa5, sizeof(value->encoding) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextCodecTransferMalformed(const UmiContextCodec *sample)
{
    (void)sample;
    {
        UmiContextCodec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.codec_id, 'x', sizeof(invalid.codec_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_codec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_codec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated codec_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextCodec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_codec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_codec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextCodec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.media_type, 'x', sizeof(invalid.media_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_codec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_codec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated media_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextCodec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.encoding, 'x', sizeof(invalid.encoding));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_codec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_codec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated encoding was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextCodecTransferCases, UmiContextCodec,
    umi_context_codec_archive_encode, umi_context_codec_archive_decode,
    UmiContextCodecTransferEqual, UmiContextCodecTransferTails, UmiContextCodecTransferMalformed)

int main(void)
{
    UmiContextCodec state;
    umi_context_codec_init(&state);
    assert(umi_context_codec_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_codec_field(&state,0U),"alpha") == 0);
    assert(umi_context_codec_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_codec_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_codec_covers_sequence(&state,10U));
    assert(umi_context_codec_covers_sequence(&state,11U));
    assert(umi_context_codec_validate(&state) == UMI_STATUS_OK);
    if (UmiContextCodecTransferCases(&state) != 0) return 1;

    return 0;
}
