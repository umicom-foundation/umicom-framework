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
/*
 * Initialise workbench context host observation from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_observation_init(
    UmiWorkbenchContextHostObservation *observation)
{/* Apply this branch only when its contract condition is satisfied. */ if(observation){memset(observation,0,sizeof(*observation));observation->structure_size=(uint32_t)sizeof(*observation);}}
/*
 * Provide the workbench context host observation diff operation used by this module and
 * its client applications.
 */
uint32_t umi_workbench_context_host_observation_diff(
    const UmiWorkbenchContextHostObservation *previous,
    const UmiWorkbenchContextHostObservation *current)
{
    uint32_t flags=0U;/* Apply this branch only when its contract condition is satisfied. */ if(!previous||!current)return UINT32_MAX;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(strcmp(previous->workspace_profile,current->workspace_profile)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_WORKSPACE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(strcmp(previous->perspective_id,current->perspective_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_PERSPECTIVE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(strcmp(previous->document_id,current->document_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_DOCUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(strcmp(previous->activity_id,current->activity_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_ACTIVITY;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(strcmp(previous->view_container_id,current->view_container_id)!=0)flags|=UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_VIEW_CONTAINER;
    return flags;
}
/*
 * Provide the workbench context host observation equal operation used by this module and
 * its client applications.
 */
bool umi_workbench_context_host_observation_equal(
    const UmiWorkbenchContextHostObservation *left,
    const UmiWorkbenchContextHostObservation *right)
{return left&&right&&umi_workbench_context_host_observation_diff(left,right)==0U;}
