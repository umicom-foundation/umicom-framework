/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_subscription_service.h
 *
 * PURPOSE:
 *   Coordinate subscription lifecycle and subscriber checkpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SUBSCRIPTION_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SUBSCRIPTION_SERVICE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context subscription service data shared with callers of this public
 * contract.
 */
typedef struct UmiContextSubscriptionService {
    uint32_t structure_size;
    char operation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subscription_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char panel_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSubscriptionService;
/**
 * Initialise context subscription service from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_subscription_service_init(UmiContextSubscriptionService *state);
/**
 * Provide the context subscription service set field operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_subscription_service_set_field(UmiContextSubscriptionService *state,size_t field_index,const char *value);
/**
 * Provide the context subscription service field operation used by this module and its
 * client applications.
 */
const char *umi_context_subscription_service_field(const UmiContextSubscriptionService *state,size_t field_index);
/**
 * Provide the context subscription service record success operation used by this module
 * and its client applications.
 */
UmiStatus umi_context_subscription_service_record_success(UmiContextSubscriptionService *state,uint64_t sequence);
/**
 * Provide the context subscription service record failure operation used by this module
 * and its client applications.
 */
UmiStatus umi_context_subscription_service_record_failure(UmiContextSubscriptionService *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context subscription service satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_context_subscription_service_validate(const UmiContextSubscriptionService *state);
/**
 * Provide the context subscription service covers sequence operation used by this module
 * and its client applications.
 */
bool umi_context_subscription_service_covers_sequence(const UmiContextSubscriptionService *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
