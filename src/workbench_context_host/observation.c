/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/observation.c
 *
 * PURPOSE:
 *   Implement deterministic workbench observation comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/observation.h"
#include <string.h>
void umi_workbench_context_host_observation_init(
    UmiWorkbenchContextHostObservation *observation)
{if(observation){memset(observation,0,sizeof(*observation));observation->structure_size=(uint32_t)sizeof(*observation);}}
uint32_t umi_workbench_context_host_observation_diff(
    const UmiWorkbenchContextHostObservation *previous,
    const UmiWorkbenchContextHostObservation *current)
{
    uint32_t flags=0U;if(!previous||!current)return UINT32_MAX;
    if(strcmp(previous->workspace_profile,current->workspace_profile)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_WORKSPACE;
    if(strcmp(previous->perspective_id,current->perspective_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_PERSPECTIVE;
    if(strcmp(previous->document_id,current->document_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_DOCUMENT;
    if(strcmp(previous->activity_id,current->activity_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_ACTIVITY;
    if(strcmp(previous->view_container_id,current->view_container_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_VIEW_CONTAINER;
    return flags;
}
bool umi_workbench_context_host_observation_equal(
    const UmiWorkbenchContextHostObservation *left,
    const UmiWorkbenchContextHostObservation *right)
{return left&&right&&umi_workbench_context_host_observation_diff(left,right)==0U;}
