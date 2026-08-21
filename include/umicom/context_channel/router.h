/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/router.h
 *
 * PURPOSE:
 *   Build deterministic delivery plans from subscriptions, routes and policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiContextDeliveryTarget { char subscription_id[UMI_CONTEXT_TEXT_CAPACITY]; char application_id[UMI_CONTEXT_TEXT_CAPACITY]; char panel_id[UMI_CONTEXT_TEXT_CAPACITY]; char channel_id[UMI_CONTEXT_TEXT_CAPACITY]; } UmiContextDeliveryTarget;
typedef struct UmiContextRoutePlan { UmiContextDeliveryTarget targets[UMI_CONTEXT_MAX_DELIVERIES]; size_t target_count; size_t rejected_count; size_t duplicate_count; uint64_t sequence; } UmiContextRoutePlan;
typedef struct UmiContextRouter { UmiContextRouteStore routes; UmiContextSubscriptionStore subscriptions; UmiContextPolicy *policy; UmiContextDeduplication deduplication; uint64_t sequence; } UmiContextRouter;
void umi_context_router_init(UmiContextRouter *router,UmiContextPolicy *policy);
UmiStatus umi_context_router_add_route(UmiContextRouter *router,const UmiContextRoute *route);
UmiStatus umi_context_router_subscribe(UmiContextRouter *router,const UmiContextSubscription *subscription);
UmiStatus umi_context_router_plan(UmiContextRouter *router,const char *source_channel_id,const UmiContextPayload *payload,UmiContextRoutePlan *out_plan);
#ifdef __cplusplus
}
#endif
#endif
