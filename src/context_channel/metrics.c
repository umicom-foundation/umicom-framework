/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/metrics.c
 *
 * PURPOSE:
 *   Maintain operational counters for context publication and delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/metrics.h"
#include <string.h>
/*
 * Initialise context metrics from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_metrics_init(UmiContextMetrics *m){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
memset(m,0,sizeof(*m));
m->revision=1U;
}
}
/*
 * Provide the context metrics record publish operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_publish(UmiContextMetrics *m,uint64_t now){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->published+=1U;
m->last_publish_ms=now;
m->revision+=1U;
}
}
/*
 * Provide the context metrics record route operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_route(UmiContextMetrics *m,size_t count){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->routed+=(uint64_t)count;
m->revision+=1U;
}
}
/*
 * Provide the context metrics record delivery operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_delivery(UmiContextMetrics *m,uint64_t now){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->delivered+=1U;
m->last_delivery_ms=now;
m->revision+=1U;
}
}
/*
 * Provide the context metrics record rejection operation used by this module and its
 * client applications.
 */
void umi_context_metrics_record_rejection(UmiContextMetrics *m){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->rejected+=1U;
m->revision+=1U;
}
}
/*
 * Provide the context metrics record duplicate operation used by this module and its
 * client applications.
 */
void umi_context_metrics_record_duplicate(UmiContextMetrics *m){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->duplicates+=1U;
m->revision+=1U;
}
}
/*
 * Provide the context metrics record expired operation used by this module and its client
 * applications.
 */
void umi_context_metrics_record_expired(UmiContextMetrics *m){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->expired+=1U;
m->revision+=1U;
}
}
/*
 * Provide the context metrics record policy denial operation used by this module and its
 * client applications.
 */
void umi_context_metrics_record_policy_denial(UmiContextMetrics *m){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->policy_denials+=1U;
m->revision+=1U;
}
}
/*
 * Provide the context metrics record subscriber failure operation used by this module and
 * its client applications.
 */
void umi_context_metrics_record_subscriber_failure(UmiContextMetrics *m){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(m!=NULL){
m->subscriber_failures+=1U;
m->revision+=1U;
}
}
