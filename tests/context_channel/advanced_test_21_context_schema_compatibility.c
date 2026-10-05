/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_21_context_schema_compatibility.c
 *
 * PURPOSE:
 *   Validate context schema compatibility sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_schema_compatibility.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_schema_compatibility.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextSchemaCompatibilityTransferEqual(const UmiContextSchemaCompatibility *a, const UmiContextSchemaCompatibility *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->check_id, b->check_id) == 0 &&
        strcmp(a->source_schema, b->source_schema) == 0 &&
        strcmp(a->target_schema, b->target_schema) == 0 &&
        strcmp(a->message, b->message) == 0 &&
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
static void UmiContextSchemaCompatibilityTransferTails(UmiContextSchemaCompatibility *value)
{
    (void)value;
    {
        size_t used = strlen(value->check_id) + 1U;
        memset(value->check_id + used, 0xa5, sizeof(value->check_id) - used);
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
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextSchemaCompatibilityTransferMalformed(const UmiContextSchemaCompatibility *sample)
{
    (void)sample;
    {
        UmiContextSchemaCompatibility invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.check_id, 'x', sizeof(invalid.check_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_schema_compatibility_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_schema_compatibility_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated check_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSchemaCompatibility invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_schema, 'x', sizeof(invalid.source_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_schema_compatibility_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_schema_compatibility_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_schema was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSchemaCompatibility invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_schema, 'x', sizeof(invalid.target_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_schema_compatibility_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_schema_compatibility_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_schema was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSchemaCompatibility invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_schema_compatibility_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_schema_compatibility_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextSchemaCompatibilityTransferCases, UmiContextSchemaCompatibility,
    umi_context_schema_compatibility_archive_encode, umi_context_schema_compatibility_archive_decode,
    UmiContextSchemaCompatibilityTransferEqual, UmiContextSchemaCompatibilityTransferTails, UmiContextSchemaCompatibilityTransferMalformed)

int main(void)
{
    UmiContextSchemaCompatibility state;
    umi_context_schema_compatibility_init(&state);
    assert(umi_context_schema_compatibility_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_schema_compatibility_field(&state,0U),"alpha") == 0);
    assert(umi_context_schema_compatibility_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_schema_compatibility_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_schema_compatibility_covers_sequence(&state,10U));
    assert(umi_context_schema_compatibility_covers_sequence(&state,11U));
    assert(umi_context_schema_compatibility_validate(&state) == UMI_STATUS_OK);
    if (UmiContextSchemaCompatibilityTransferCases(&state) != 0) return 1;

    return 0;
}
