/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_25_context_health.c
 *
 * PURPOSE:
 *   Validate context health sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_health.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_health.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextHealthTransferEqual(const UmiContextHealth *a, const UmiContextHealth *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->health_id, b->health_id) == 0 &&
        strcmp(a->component_id, b->component_id) == 0 &&
        strcmp(a->message, b->message) == 0 &&
        strcmp(a->last_failure, b->last_failure) == 0 &&
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
static void UmiContextHealthTransferTails(UmiContextHealth *value)
{
    (void)value;
    {
        size_t used = strlen(value->health_id) + 1U;
        memset(value->health_id + used, 0xa5, sizeof(value->health_id) - used);
    }
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
    {
        size_t used = strlen(value->last_failure) + 1U;
        memset(value->last_failure + used, 0xa5, sizeof(value->last_failure) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextHealthTransferMalformed(const UmiContextHealth *sample)
{
    (void)sample;
    {
        UmiContextHealth invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.health_id, 'x', sizeof(invalid.health_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_health_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_health_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated health_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextHealth invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_health_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_health_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextHealth invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_health_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_health_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextHealth invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.last_failure, 'x', sizeof(invalid.last_failure));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_health_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_health_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated last_failure was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextHealthTransferCases, UmiContextHealth,
    umi_context_health_archive_encode, umi_context_health_archive_decode,
    UmiContextHealthTransferEqual, UmiContextHealthTransferTails, UmiContextHealthTransferMalformed)

int main(void)
{
    UmiContextHealth state;
    umi_context_health_init(&state);
    assert(umi_context_health_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_health_field(&state,0U),"alpha") == 0);
    assert(umi_context_health_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_health_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_health_covers_sequence(&state,10U));
    assert(umi_context_health_covers_sequence(&state,11U));
    assert(umi_context_health_validate(&state) == UMI_STATUS_OK);
    if (UmiContextHealthTransferCases(&state) != 0) return 1;

    return 0;
}
