/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_23_context_filter.c
 *
 * PURPOSE:
 *   Validate context filter sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_filter.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_filter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextFilterTransferEqual(const UmiContextFilter *a, const UmiContextFilter *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->evaluation_id, b->evaluation_id) == 0 &&
        strcmp(a->filter_id, b->filter_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->field_name, b->field_name) == 0 &&
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
static void UmiContextFilterTransferTails(UmiContextFilter *value)
{
    (void)value;
    {
        size_t used = strlen(value->evaluation_id) + 1U;
        memset(value->evaluation_id + used, 0xa5, sizeof(value->evaluation_id) - used);
    }
    {
        size_t used = strlen(value->filter_id) + 1U;
        memset(value->filter_id + used, 0xa5, sizeof(value->filter_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->field_name) + 1U;
        memset(value->field_name + used, 0xa5, sizeof(value->field_name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextFilterTransferMalformed(const UmiContextFilter *sample)
{
    (void)sample;
    {
        UmiContextFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.evaluation_id, 'x', sizeof(invalid.evaluation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated evaluation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.filter_id, 'x', sizeof(invalid.filter_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated filter_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field_name, 'x', sizeof(invalid.field_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field_name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextFilterTransferCases, UmiContextFilter,
    umi_context_filter_archive_encode, umi_context_filter_archive_decode,
    UmiContextFilterTransferEqual, UmiContextFilterTransferTails, UmiContextFilterTransferMalformed)

int main(void)
{
    UmiContextFilter state;
    umi_context_filter_init(&state);
    assert(umi_context_filter_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_filter_field(&state,0U),"alpha") == 0);
    assert(umi_context_filter_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_filter_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_filter_covers_sequence(&state,10U));
    assert(umi_context_filter_covers_sequence(&state,11U));
    assert(umi_context_filter_validate(&state) == UMI_STATUS_OK);
    if (UmiContextFilterTransferCases(&state) != 0) return 1;

    return 0;
}
