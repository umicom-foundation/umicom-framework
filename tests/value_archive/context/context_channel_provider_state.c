/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_provider_state.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/provider_state.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/provider_state.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextProviderStateTransferEqual(const UmiContextProviderState *a, const UmiContextProviderState *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->provider_id, b->provider_id) == 0 &&
        strcmp(a->message, b->message) == 0 &&
        a->status == b->status &&
        a->last_success_ms == b->last_success_ms &&
        a->last_failure_ms == b->last_failure_ms &&
        a->publish_count == b->publish_count &&
        a->failure_count == b->failure_count &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextProviderStateTransferTails(UmiContextProviderState *value)
{
    (void)value;
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextProviderStateTransferMalformed(const UmiContextProviderState *sample)
{
    (void)sample;
    {
        UmiContextProviderState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_id, 'x', sizeof(invalid.provider_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_provider_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_provider_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextProviderState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_provider_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_provider_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextProviderStateTransferCases, UmiContextProviderState,
    umi_context_provider_state_archive_encode, umi_context_provider_state_archive_decode,
    UmiContextProviderStateTransferEqual, UmiContextProviderStateTransferTails, UmiContextProviderStateTransferMalformed)

int main(void)
{
    UmiContextProviderState value;
    umi_context_provider_state_init(&value);
    value.provider_id[0] = 's';
    value.message[0] = 's';
    value.last_success_ms = (uint64_t)17U;
    value.last_failure_ms = (uint64_t)17U;
    value.publish_count = (uint64_t)17U;
    value.failure_count = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_provider_state_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextProviderStateTransferCases(&value) != 0) return 1;

    return 0;
}
