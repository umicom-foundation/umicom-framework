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
void umi_context_metrics_init(UmiContextMetrics *m){
if(m!=NULL){
memset(m,0,sizeof(*m));
m->revision=1U;
}
}
void umi_context_metrics_record_publish(UmiContextMetrics *m,uint64_t now){
if(m!=NULL){
m->published+=1U;
m->last_publish_ms=now;
m->revision+=1U;
}
}
void umi_context_metrics_record_route(UmiContextMetrics *m,size_t count){
if(m!=NULL){
m->routed+=(uint64_t)count;
m->revision+=1U;
}
}
void umi_context_metrics_record_delivery(UmiContextMetrics *m,uint64_t now){
if(m!=NULL){
m->delivered+=1U;
m->last_delivery_ms=now;
m->revision+=1U;
}
}
void umi_context_metrics_record_rejection(UmiContextMetrics *m){
if(m!=NULL){
m->rejected+=1U;
m->revision+=1U;
}
}
void umi_context_metrics_record_duplicate(UmiContextMetrics *m){
if(m!=NULL){
m->duplicates+=1U;
m->revision+=1U;
}
}
void umi_context_metrics_record_expired(UmiContextMetrics *m){
if(m!=NULL){
m->expired+=1U;
m->revision+=1U;
}
}
void umi_context_metrics_record_policy_denial(UmiContextMetrics *m){
if(m!=NULL){
m->policy_denials+=1U;
m->revision+=1U;
}
}
void umi_context_metrics_record_subscriber_failure(UmiContextMetrics *m){
if(m!=NULL){
m->subscriber_failures+=1U;
m->revision+=1U;
}
}
