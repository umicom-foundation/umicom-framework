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
/*
 * Initialise workbench context host throttle from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_throttle_init(UmiWorkbenchContextHostThrottle *t,uint64_t interval)
{/* Apply this branch only when its contract condition is satisfied. */ if(t){t->minimum_interval_ms=interval;t->last_accepted_ms=0U;t->accepted_count=0U;t->throttled_count=0U;}}
/*
 * Provide the workbench context host throttle accept operation used by this module and its
 * client applications.
 */
bool umi_workbench_context_host_throttle_accept(UmiWorkbenchContextHostThrottle *t,uint64_t now_ms)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if(!t)return false;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(t->accepted_count>0U&&now_ms>=t->last_accepted_ms&&now_ms-t->last_accepted_ms<t->minimum_interval_ms){
        ++t->throttled_count;return false;}
    t->last_accepted_ms=now_ms;++t->accepted_count;return true;
}
