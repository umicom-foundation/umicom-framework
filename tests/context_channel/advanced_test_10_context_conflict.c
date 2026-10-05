/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_10_context_conflict.c
 *
 * PURPOSE:
 *   Validate context conflict sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_conflict.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_conflict.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextConflictTransferEqual(const UmiContextConflict *a, const UmiContextConflict *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->conflict_id, b->conflict_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->local_schema, b->local_schema) == 0 &&
        strcmp(a->remote_schema, b->remote_schema) == 0 &&
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
static void UmiContextConflictTransferTails(UmiContextConflict *value)
{
    (void)value;
    {
        size_t used = strlen(value->conflict_id) + 1U;
        memset(value->conflict_id + used, 0xa5, sizeof(value->conflict_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->local_schema) + 1U;
        memset(value->local_schema + used, 0xa5, sizeof(value->local_schema) - used);
    }
    {
        size_t used = strlen(value->remote_schema) + 1U;
        memset(value->remote_schema + used, 0xa5, sizeof(value->remote_schema) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextConflictTransferMalformed(const UmiContextConflict *sample)
{
    (void)sample;
    {
        UmiContextConflict invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.conflict_id, 'x', sizeof(invalid.conflict_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_conflict_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_conflict_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated conflict_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextConflict invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_conflict_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_conflict_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextConflict invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.local_schema, 'x', sizeof(invalid.local_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_conflict_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_conflict_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated local_schema was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextConflict invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.remote_schema, 'x', sizeof(invalid.remote_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_conflict_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_conflict_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated remote_schema was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextConflictTransferCases, UmiContextConflict,
    umi_context_conflict_archive_encode, umi_context_conflict_archive_decode,
    UmiContextConflictTransferEqual, UmiContextConflictTransferTails, UmiContextConflictTransferMalformed)

int main(void)
{
    UmiContextConflict state;
    umi_context_conflict_init(&state);
    assert(umi_context_conflict_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_conflict_field(&state,0U),"alpha") == 0);
    assert(umi_context_conflict_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_conflict_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_conflict_covers_sequence(&state,10U));
    assert(umi_context_conflict_covers_sequence(&state,11U));
    assert(umi_context_conflict_validate(&state) == UMI_STATUS_OK);
    if (UmiContextConflictTransferCases(&state) != 0) return 1;

    return 0;
}
