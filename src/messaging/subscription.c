/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/subscription.c
 *
 * PURPOSE:
 *   Implement exact and prefix subscription matching across kind, name, schema, source and partition fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/subscription.h"

#include <string.h>

UmiSubscription umi_subscription_all(void)
{
    UmiSubscription subscription;
    (void)memset(&subscription, 0, sizeof(subscription));
    subscription.structure_size = (uint32_t)sizeof(subscription);
    return subscription;
}

static int text_matches(const char *expected,
                        const char *actual,
                        int prefix_match)
{
    size_t expected_length;
    if (expected == NULL || expected[0] == '\0') return 1;
    if (actual == NULL) return 0;
    if (!prefix_match) return strcmp(expected, actual) == 0;
    expected_length = strlen(expected);
    return strncmp(expected, actual, expected_length) == 0;
}

int umi_subscription_matches(const UmiSubscription *subscription,
                             const UmiMessageEnvelope *message)
{
    if (subscription == NULL || message == NULL) return 0;
    if (subscription->kind != 0 && subscription->kind != message->kind) {
        return 0;
    }
    return text_matches(subscription->name,
                        message->name,
                        subscription->prefix_match) &&
           text_matches(subscription->schema_id,
                        message->schema_id,
                        0) &&
           text_matches(subscription->source,
                        message->source,
                        0) &&
           text_matches(subscription->partition_key,
                        message->partition_key,
                        0);
}
