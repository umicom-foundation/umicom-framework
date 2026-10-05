/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_26_context_profile.c
 *
 * PURPOSE:
 *   Validate context profile sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextProfileTransferEqual(const UmiContextProfile *a, const UmiContextProfile *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->profile_id, b->profile_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->default_channel, b->default_channel) == 0 &&
        strcmp(a->default_schema, b->default_schema) == 0 &&
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
static void UmiContextProfileTransferTails(UmiContextProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->default_channel) + 1U;
        memset(value->default_channel + used, 0xa5, sizeof(value->default_channel) - used);
    }
    {
        size_t used = strlen(value->default_schema) + 1U;
        memset(value->default_schema + used, 0xa5, sizeof(value->default_schema) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextProfileTransferMalformed(const UmiContextProfile *sample)
{
    (void)sample;
    {
        UmiContextProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.default_channel, 'x', sizeof(invalid.default_channel));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated default_channel was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.default_schema, 'x', sizeof(invalid.default_schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated default_schema was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextProfileTransferCases, UmiContextProfile,
    umi_context_profile_archive_encode, umi_context_profile_archive_decode,
    UmiContextProfileTransferEqual, UmiContextProfileTransferTails, UmiContextProfileTransferMalformed)

int main(void)
{
    UmiContextProfile state;
    umi_context_profile_init(&state);
    assert(umi_context_profile_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_profile_field(&state,0U),"alpha") == 0);
    assert(umi_context_profile_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_profile_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_profile_covers_sequence(&state,10U));
    assert(umi_context_profile_covers_sequence(&state,11U));
    assert(umi_context_profile_validate(&state) == UMI_STATUS_OK);
    if (UmiContextProfileTransferCases(&state) != 0) return 1;

    return 0;
}
