/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_11_context_merge.c
 *
 * PURPOSE:
 *   Validate context merge sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_merge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_merge.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextMergeTransferEqual(const UmiContextMerge *a, const UmiContextMerge *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->merge_id, b->merge_id) == 0 &&
        strcmp(a->conflict_id, b->conflict_id) == 0 &&
        strcmp(a->resolution, b->resolution) == 0 &&
        strcmp(a->result_context_id, b->result_context_id) == 0 &&
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
static void UmiContextMergeTransferTails(UmiContextMerge *value)
{
    (void)value;
    {
        size_t used = strlen(value->merge_id) + 1U;
        memset(value->merge_id + used, 0xa5, sizeof(value->merge_id) - used);
    }
    {
        size_t used = strlen(value->conflict_id) + 1U;
        memset(value->conflict_id + used, 0xa5, sizeof(value->conflict_id) - used);
    }
    {
        size_t used = strlen(value->resolution) + 1U;
        memset(value->resolution + used, 0xa5, sizeof(value->resolution) - used);
    }
    {
        size_t used = strlen(value->result_context_id) + 1U;
        memset(value->result_context_id + used, 0xa5, sizeof(value->result_context_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextMergeTransferMalformed(const UmiContextMerge *sample)
{
    (void)sample;
    {
        UmiContextMerge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.merge_id, 'x', sizeof(invalid.merge_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_merge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_merge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated merge_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextMerge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.conflict_id, 'x', sizeof(invalid.conflict_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_merge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_merge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated conflict_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextMerge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resolution, 'x', sizeof(invalid.resolution));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_merge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_merge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resolution was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextMerge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.result_context_id, 'x', sizeof(invalid.result_context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_merge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_merge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated result_context_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextMergeTransferCases, UmiContextMerge,
    umi_context_merge_archive_encode, umi_context_merge_archive_decode,
    UmiContextMergeTransferEqual, UmiContextMergeTransferTails, UmiContextMergeTransferMalformed)

int main(void)
{
    UmiContextMerge state;
    umi_context_merge_init(&state);
    assert(umi_context_merge_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_merge_field(&state,0U),"alpha") == 0);
    assert(umi_context_merge_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_merge_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_merge_covers_sequence(&state,10U));
    assert(umi_context_merge_covers_sequence(&state,11U));
    assert(umi_context_merge_validate(&state) == UMI_STATUS_OK);
    if (UmiContextMergeTransferCases(&state) != 0) return 1;

    return 0;
}
