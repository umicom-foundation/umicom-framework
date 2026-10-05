/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_22_context_transform.c
 *
 * PURPOSE:
 *   Validate context transform sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_transform.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_transform.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextTransformTransferEqual(const UmiContextTransform *a, const UmiContextTransform *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->plan_id, b->plan_id) == 0 &&
        strcmp(a->source_schema, b->source_schema) == 0 &&
        strcmp(a->target_schema, b->target_schema) == 0 &&
        strcmp(a->transformer_id, b->transformer_id) == 0 &&
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
static void UmiContextTransformTransferTails(UmiContextTransform *value)
{
    (void)value;
    {
        size_t used = strlen(value->plan_id) + 1U;
        memset(value->plan_id + used, 0xa5, sizeof(value->plan_id) - used);
    }
    {
        size_t used = strlen(value->source_schema) + 1U;
        memset(value->source_schema + used, 0xa5, sizeof(value->source_schema) - used);
    }
    {
        size_t used = strlen(value->target_schema) + 1U;
        memset(value->target_schema + used, 0xa5, sizeof(value->target_schema) - used);
    }
    {
        size_t used = strlen(value->transformer_id) + 1U;
        memset(value->transformer_id + used, 0xa5, sizeof(value->transformer_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextTransformTransferMalformed(const UmiContextTransform *sample)
{
    (void)sample;
    {
        UmiContextTransform invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.plan_id, 'x', sizeof(invalid.plan_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transform_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transform_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated plan_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransform invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_schema, 'x', sizeof(invalid.source_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transform_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transform_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_schema was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransform invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_schema, 'x', sizeof(invalid.target_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transform_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transform_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_schema was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransform invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transformer_id, 'x', sizeof(invalid.transformer_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transform_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transform_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transformer_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextTransformTransferCases, UmiContextTransform,
    umi_context_transform_archive_encode, umi_context_transform_archive_decode,
    UmiContextTransformTransferEqual, UmiContextTransformTransferTails, UmiContextTransformTransferMalformed)

int main(void)
{
    UmiContextTransform state;
    umi_context_transform_init(&state);
    assert(umi_context_transform_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_transform_field(&state,0U),"alpha") == 0);
    assert(umi_context_transform_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_transform_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_transform_covers_sequence(&state,10U));
    assert(umi_context_transform_covers_sequence(&state,11U));
    assert(umi_context_transform_validate(&state) == UMI_STATUS_OK);
    if (UmiContextTransformTransferCases(&state) != 0) return 1;

    return 0;
}
