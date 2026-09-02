/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/debounce_policy.c
 *
 * PURPOSE:
 *   Provide deterministic deadline calculation for debounced updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/debounce_policy.h"
/* Record an event and calculate the next deterministic debounce deadline. */
uint64_t umi_ui_reactive_debounce_policy_record(UmiUiReactiveDebouncePolicy *p,uint64_t now){/* Apply this operation only while the related capability or state is available. */ if(!p)return 0U;p->last_event_millis=now;p->due_millis=now+p->delay_millis;return p->due_millis;}
/* Report whether a debounced update is due. */
int umi_ui_reactive_debounce_policy_due(const UmiUiReactiveDebouncePolicy *p,uint64_t now){return p!=NULL&&p->due_millis!=0U&&now>=p->due_millis;}
