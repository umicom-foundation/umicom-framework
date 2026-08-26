/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/alert_state.c
 *
 * PURPOSE:
 *   Track alert activation, occurrence and acknowledgement state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/alert_state.h"

#include <string.h>
void umi_analytics_alert_state_init(UmiAnalyticsAlertState *s){if(s!=NULL)memset(s,0,sizeof *s);}
void umi_analytics_alert_state_apply(UmiAnalyticsAlertState *s,int triggered,int64_t now_ns){if(s==NULL)return;if(triggered){if(!s->active){s->active=1;s->active_since_ns=now_ns;s->acknowledged=0;}s->occurrences++;}else{s->active=0;s->active_since_ns=0;s->acknowledged=0;}}
void umi_analytics_alert_state_acknowledge(UmiAnalyticsAlertState *s){if(s!=NULL&&s->active)s->acknowledged=1;}
