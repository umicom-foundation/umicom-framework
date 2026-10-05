/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_27_context_preference.c
 *
 * PURPOSE:
 *   Validate context preference sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_preference.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_preference.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextPreferenceTransferEqual(const UmiContextPreference *a, const UmiContextPreference *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->preference_id, b->preference_id) == 0 &&
        strcmp(a->user_id, b->user_id) == 0 &&
        strcmp(a->default_colour, b->default_colour) == 0 &&
        strcmp(a->default_channel, b->default_channel) == 0 &&
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
static void UmiContextPreferenceTransferTails(UmiContextPreference *value)
{
    (void)value;
    {
        size_t used = strlen(value->preference_id) + 1U;
        memset(value->preference_id + used, 0xa5, sizeof(value->preference_id) - used);
    }
    {
        size_t used = strlen(value->user_id) + 1U;
        memset(value->user_id + used, 0xa5, sizeof(value->user_id) - used);
    }
    {
        size_t used = strlen(value->default_colour) + 1U;
        memset(value->default_colour + used, 0xa5, sizeof(value->default_colour) - used);
    }
    {
        size_t used = strlen(value->default_channel) + 1U;
        memset(value->default_channel + used, 0xa5, sizeof(value->default_channel) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextPreferenceTransferMalformed(const UmiContextPreference *sample)
{
    (void)sample;
    {
        UmiContextPreference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.preference_id, 'x', sizeof(invalid.preference_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_preference_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_preference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated preference_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextPreference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.user_id, 'x', sizeof(invalid.user_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_preference_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_preference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated user_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextPreference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.default_colour, 'x', sizeof(invalid.default_colour));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_preference_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_preference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated default_colour was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextPreference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.default_channel, 'x', sizeof(invalid.default_channel));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_preference_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_preference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated default_channel was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextPreferenceTransferCases, UmiContextPreference,
    umi_context_preference_archive_encode, umi_context_preference_archive_decode,
    UmiContextPreferenceTransferEqual, UmiContextPreferenceTransferTails, UmiContextPreferenceTransferMalformed)

int main(void)
{
    UmiContextPreference state;
    umi_context_preference_init(&state);
    assert(umi_context_preference_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_preference_field(&state,0U),"alpha") == 0);
    assert(umi_context_preference_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_preference_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_preference_covers_sequence(&state,10U));
    assert(umi_context_preference_covers_sequence(&state,11U));
    assert(umi_context_preference_validate(&state) == UMI_STATUS_OK);
    if (UmiContextPreferenceTransferCases(&state) != 0) return 1;

    return 0;
}
