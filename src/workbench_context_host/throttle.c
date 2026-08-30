/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/throttle.c
 *
 * PURPOSE:
 *   Apply monotonic publication spacing and count throttled observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/throttle.h"
void umi_workbench_context_host_throttle_init(UmiWorkbenchContextHostThrottle *t,uint64_t interval)
{if(t){t->minimum_interval_ms=interval;t->last_accepted_ms=0U;t->accepted_count=0U;t->throttled_count=0U;}}
bool umi_workbench_context_host_throttle_accept(UmiWorkbenchContextHostThrottle *t,uint64_t now_ms)
{
    if(!t)return false;
    if(t->accepted_count>0U&&now_ms>=t->last_accepted_ms&&now_ms-t->last_accepted_ms<t->minimum_interval_ms){
        ++t->throttled_count;return false;}
    t->last_accepted_ms=now_ms;++t->accepted_count;return true;
}
