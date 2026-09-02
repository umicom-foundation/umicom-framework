/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/alert_state.c
 *
 * PURPOSE:
 *   Track alert activation, occurrence and acknowledgement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/alert_state.h"

#include <string.h>
/*
 * Initialise analytics alert state from caller-provided values so later operations receive
 * a known state.
 */
void umi_analytics_alert_state_init(UmiAnalyticsAlertState *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Perform analytics alert state through the module contract so client applications do not
 * duplicate its policy.
 */
void umi_analytics_alert_state_apply(UmiAnalyticsAlertState *s,int triggered,int64_t now_ns){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(triggered){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s->active){s->active=1;s->active_since_ns=now_ns;s->acknowledged=0;}s->occurrences++;}/* Use this fallback path when the earlier condition does not apply. */ else{s->active=0;s->active_since_ns=0;s->acknowledged=0;}}
/*
 * Provide the analytics alert state acknowledge operation used by this module and its
 * client applications.
 */
void umi_analytics_alert_state_acknowledge(UmiAnalyticsAlertState *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL&&s->active)s->acknowledged=1;}
