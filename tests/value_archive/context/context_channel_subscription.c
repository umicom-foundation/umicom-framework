/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_subscription.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/subscription.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/subscription.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextSubscriptionTransferEqual(const UmiContextSubscription *a, const UmiContextSubscription *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->subscription_id, b->subscription_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        a->role == b->role &&
        a->enabled == b->enabled &&
        a->last_sequence == b->last_sequence &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextSubscriptionTransferTails(UmiContextSubscription *value)
{
    (void)value;
    {
        size_t used = strlen(value->subscription_id) + 1U;
        memset(value->subscription_id + used, 0xa5, sizeof(value->subscription_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextSubscriptionTransferMalformed(const UmiContextSubscription *sample)
{
    (void)sample;
    {
        UmiContextSubscription invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subscription_id, 'x', sizeof(invalid.subscription_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_subscription_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_subscription_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subscription_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSubscription invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_subscription_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_subscription_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSubscription invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_subscription_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_subscription_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSubscription invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_subscription_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_subscription_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextSubscriptionTransferCases, UmiContextSubscription,
    umi_context_subscription_archive_encode, umi_context_subscription_archive_decode,
    UmiContextSubscriptionTransferEqual, UmiContextSubscriptionTransferTails, UmiContextSubscriptionTransferMalformed)

int main(void)
{
    UmiContextSubscription value;
    umi_context_subscription_init(&value);
    value.subscription_id[0] = 's';
    value.channel_id[0] = 's';
    value.application_id[0] = 's';
    value.panel_id[0] = 's';
    value.enabled = true;
    value.last_sequence = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_subscription_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextSubscriptionTransferCases(&value) != 0) return 1;

    return 0;
}
