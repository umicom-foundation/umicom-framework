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
void umi_workbench_context_host_observer_init(UmiWorkbenchContextHostObserver *observer)
{if(observer){memset(observer,0,sizeof(*observer));observer->revision=1U;}}
uint32_t umi_workbench_context_host_observer_push(
    UmiWorkbenchContextHostObserver *observer,
    const UmiWorkbenchContextHostObservation *observation)
{
    uint32_t flags;
    if(!observer||!observation)return UINT32_MAX;
    flags=observer->has_last?umi_workbench_context_host_observation_diff(&observer->last,observation):UINT32_MAX;
    if(observer->has_last&&flags==0U)++observer->duplicate_count;
    else{observer->last=*observation;observer->has_last=true;++observer->revision;}
    ++observer->observation_count;return flags;
}
