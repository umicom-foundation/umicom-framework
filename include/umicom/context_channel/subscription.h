/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/subscription.h
 *
 * PURPOSE:
 *   Track panel subscriptions without direct panel-to-panel pointers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SUBSCRIPTION_H
#define UMICOM_CONTEXT_CHANNEL_SUBSCRIPTION_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_SUBSCRIPTION_MAX_ITEMS 256U
typedef struct UmiContextSubscription {
    uint32_t structure_size;
    char subscription_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    UmiContextSubscriptionRole role;
    bool enabled;
    uint64_t last_sequence;
    uint64_t revision;
} UmiContextSubscription;
typedef struct UmiContextSubscriptionStore {
    UmiContextSubscription items[UMI_CONTEXT_SUBSCRIPTION_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextSubscriptionStore;
void umi_context_subscription_init(UmiContextSubscription *record);
UmiStatus umi_context_subscription_validate(const UmiContextSubscription *record);
void umi_context_subscription_store_init(UmiContextSubscriptionStore *store);
UmiStatus umi_context_subscription_store_put(UmiContextSubscriptionStore *store,const UmiContextSubscription *record);
UmiStatus umi_context_subscription_store_remove(UmiContextSubscriptionStore *store,const char *identity);
UmiContextSubscription *umi_context_subscription_store_find(UmiContextSubscriptionStore *store,const char *identity);
const UmiContextSubscription *umi_context_subscription_store_find_const(const UmiContextSubscriptionStore *store,const char *identity);
size_t umi_context_subscription_store_count(const UmiContextSubscriptionStore *store);
UmiStatus umi_context_subscription_store_snapshot(const UmiContextSubscriptionStore *store,UmiContextSubscription *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
