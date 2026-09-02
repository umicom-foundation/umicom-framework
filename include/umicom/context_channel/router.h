/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/router.h
 *
 * PURPOSE:
 *   Build deterministic delivery plans from subscriptions, routes and policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_ROUTER_H
#define UMICOM_CONTEXT_CHANNEL_ROUTER_H
#include "umicom/context_channel/route.h"
#include "umicom/context_channel/subscription.h"
#include "umicom/context_channel/policy.h"
#include "umicom/context_channel/deduplication.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context delivery target data shared with callers of this public contract.
 */
typedef struct UmiContextDeliveryTarget { char subscription_id[UMI_CONTEXT_TEXT_CAPACITY]; char application_id[UMI_CONTEXT_TEXT_CAPACITY]; char panel_id[UMI_CONTEXT_TEXT_CAPACITY]; char channel_id[UMI_CONTEXT_TEXT_CAPACITY]; } UmiContextDeliveryTarget;
/**
 * Represent the context route plan data shared with callers of this public contract.
 */
typedef struct UmiContextRoutePlan { UmiContextDeliveryTarget targets[UMI_CONTEXT_MAX_DELIVERIES]; size_t target_count; size_t rejected_count; size_t duplicate_count; uint64_t sequence; } UmiContextRoutePlan;
/**
 * Represent the context router data shared with callers of this public contract.
 */
typedef struct UmiContextRouter { UmiContextRouteStore routes; UmiContextSubscriptionStore subscriptions; UmiContextPolicy *policy; UmiContextDeduplication deduplication; uint64_t sequence; } UmiContextRouter;
/**
 * Initialise context router from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_router_init(UmiContextRouter *router,UmiContextPolicy *policy);
/**
 * Provide the context router add route operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_router_add_route(UmiContextRouter *router,const UmiContextRoute *route);
/**
 * Provide the context router subscribe operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_router_subscribe(UmiContextRouter *router,const UmiContextSubscription *subscription);
/**
 * Provide the context router plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_router_plan(UmiContextRouter *router,const char *source_channel_id,const UmiContextPayload *payload,UmiContextRoutePlan *out_plan);
#ifdef __cplusplus
}
#endif
#endif
