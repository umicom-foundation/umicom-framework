/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/throttle_policy.c
 *
 * PURPOSE:
 *   Provide deterministic admission rules for throttled updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/throttle_policy.h"
/* Admit an update only after the throttle interval has elapsed. */
int umi_ui_reactive_throttle_policy_admit(UmiUiReactiveThrottlePolicy *p,uint64_t now){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p)return 0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->last_dispatch_millis==0U||now-p->last_dispatch_millis>=p->interval_millis){p->last_dispatch_millis=now;return 1;}return 0;}
