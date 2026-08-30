/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/subscription.h
 *
 * PURPOSE:
 *   Define typed message-subscription criteria shared by dispatchers, routers, durable consumers and Studio inspectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_SUBSCRIPTION_H
#define UMICOM_MESSAGING_SUBSCRIPTION_H

#include <stdint.h>

#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSubscription {
    uint32_t structure_size;
    UmiMessageKind kind;
    const char *name;
    const char *schema_id;
    const char *source;
    const char *partition_key;
    int prefix_match;
} UmiSubscription;

UmiSubscription umi_subscription_all(void);
int umi_subscription_matches(const UmiSubscription *subscription,
                             const UmiMessageEnvelope *message);

#ifdef __cplusplus
}
#endif

#endif
