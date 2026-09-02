/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/router.c
 *
 * PURPOSE:
 *   Build deterministic delivery plans from subscriptions and policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/router.h"
#include <string.h>
/*
 * Initialise context router from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_router_init(UmiContextRouter *router,UmiContextPolicy *policy){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(router==NULL)return;
memset(router,0,sizeof(*router));
umi_context_route_store_init(&router->routes);
umi_context_subscription_store_init(&router->subscriptions);
umi_context_deduplication_init(&router->deduplication);
router->policy=policy;
router->sequence=1U;
}
/*
 * Provide the context router add route operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_router_add_route(UmiContextRouter *router,const UmiContextRoute *route){
return router==NULL||route==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_context_route_store_put(&router->routes,route);
}
/*
 * Provide the context router subscribe operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_router_subscribe(UmiContextRouter *router,const UmiContextSubscription *subscription){
return router==NULL||subscription==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_context_subscription_store_put(&router->subscriptions,subscription);
}
/*
 * Provide the context router plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_router_plan(UmiContextRouter *router,const char *channel_id,const UmiContextPayload *payload,UmiContextRoutePlan *out){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(router==NULL||channel_id==NULL||payload==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
memset(out,0,sizeof(*out));
/* Apply this branch only when its contract condition is satisfied. */
if(umi_context_deduplication_seen(&router->deduplication,payload)){
out->duplicate_count=1U;
return UMI_STATUS_ALREADY_EXISTS;
}
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<router->subscriptions.count;++i){
const UmiContextSubscription *s=&router->subscriptions.items[i];
UmiContextPolicyResult p;
/* Use the stable identifier comparison to choose the matching record or policy. */
if(!s->enabled||strcmp(s->channel_id,channel_id)!=0||s->role==UMI_CONTEXT_ROLE_PUBLISHER)continue;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(router->policy!=NULL&&umi_context_policy_evaluate(router->policy,payload,s->application_id,&p)==UMI_STATUS_OK&&p.decision==UMI_CONTEXT_POLICY_DENY){
out->rejected_count+=1U;
continue;
}
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(out->target_count>=UMI_CONTEXT_MAX_DELIVERIES)return UMI_STATUS_CAPACITY_EXCEEDED;
{
UmiContextDeliveryTarget *t=&out->targets[out->target_count++];
(void)umi_context_copy_text(t->subscription_id,sizeof(t->subscription_id),s->subscription_id);
(void)umi_context_copy_text(t->application_id,sizeof(t->application_id),s->application_id);
(void)umi_context_copy_text(t->panel_id,sizeof(t->panel_id),s->panel_id);
(void)umi_context_copy_text(t->channel_id,sizeof(t->channel_id),s->channel_id);
}
}
(void)umi_context_deduplication_record(&router->deduplication,payload);
out->sequence=router->sequence++;
return UMI_STATUS_OK;
}
