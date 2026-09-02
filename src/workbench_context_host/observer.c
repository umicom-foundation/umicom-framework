/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/observer.c
 *
 * PURPOSE:
 *   Implement observation retention, change detection and duplicate accounting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/observer.h"
#include <string.h>
/*
 * Initialise workbench context host observer from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_observer_init(UmiWorkbenchContextHostObserver *observer)
{/* Apply this branch only when its contract condition is satisfied. */ if(observer){memset(observer,0,sizeof(*observer));observer->revision=1U;}}
/*
 * Provide the workbench context host observer push operation used by this module and its
 * client applications.
 */
uint32_t umi_workbench_context_host_observer_push(
    UmiWorkbenchContextHostObserver *observer,
    const UmiWorkbenchContextHostObservation *observation)
{
    uint32_t flags;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!observer||!observation)return UINT32_MAX;
    flags=observer->has_last?umi_workbench_context_host_observation_diff(&observer->last,observation):UINT32_MAX;
    /* Apply this branch only when its contract condition is satisfied. */
    if(observer->has_last&&flags==0U)++observer->duplicate_count;
    /* Use this fallback path when the earlier condition does not apply. */
    else{observer->last=*observation;observer->has_last=true;++observer->revision;}
    ++observer->observation_count;return flags;
}
