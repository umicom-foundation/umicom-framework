/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/focus_propagation.c
 *
 * PURPOSE:
 *   Apply conservative user-focus preservation while allowing semantic reveal/activation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/focus_propagation.h"
/*
 * Provide the workbench context host focus policy default operation used by this module
 * and its client applications.
 */
UmiWorkbenchContextHostFocusPolicy umi_workbench_context_host_focus_policy_default(void)
{
    UmiWorkbenchContextHostFocusPolicy p={true,true,true,true};return p;
}
/*
 * Provide the workbench context host focus decide operation used by this module and its
 * client applications.
 */
UmiWorkbenchContextHostFocusAction umi_workbench_context_host_focus_decide(
    const UmiWorkbenchContextHostFocusPolicy *policy,const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind kind,bool visible,bool active)
{
    /* Apply this operation only while the related capability or state is available. */
    if(!policy||!endpoint||!endpoint->enabled)return UMI_WORKBENCH_CONTEXT_HOST_FOCUS_NONE;
    /* Apply this operation only while the related capability or state is available. */
    if(active)return UMI_WORKBENCH_CONTEXT_HOST_FOCUS_NONE;
    /* Apply this operation only while the related capability or state is available. */
    if(!visible&&policy->reveal_hidden_followers)return UMI_WORKBENCH_CONTEXT_HOST_FOCUS_REVEAL;
    /* Apply this branch only when its contract condition is satisfied. */
    if(policy->preserve_user_focus)return UMI_WORKBENCH_CONTEXT_HOST_FOCUS_NONE;
    /* Apply this branch only when its contract condition is satisfied. */
    if(kind==UMI_CONTEXT_KIND_SOURCE_LOCATION&&endpoint->role==UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR&&
       policy->activate_editor_for_source_location)return UMI_WORKBENCH_CONTEXT_HOST_FOCUS_ACTIVATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if(kind==UMI_CONTEXT_KIND_INSTRUMENT&&endpoint->role==UMI_WORKBENCH_CONTEXT_HOST_PANEL_CHART&&
       policy->activate_chart_for_instrument)return UMI_WORKBENCH_CONTEXT_HOST_FOCUS_ACTIVATE;
    return UMI_WORKBENCH_CONTEXT_HOST_FOCUS_NONE;
}
