/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/focus_propagation.h
 *
 * PURPOSE:
 *   Decide whether a delivered context should activate, reveal or leave a receiving panel unchanged.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_FOCUS_PROPAGATION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_FOCUS_PROPAGATION_H
#include "umicom/workbench_context_host/endpoint.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named workbench context host focus action values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostFocusAction {
    UMI_WORKBENCH_CONTEXT_HOST_FOCUS_NONE=0,
    UMI_WORKBENCH_CONTEXT_HOST_FOCUS_REVEAL=1,
    UMI_WORKBENCH_CONTEXT_HOST_FOCUS_ACTIVATE=2
} UmiWorkbenchContextHostFocusAction;
/**
 * Represent the workbench context host focus policy data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostFocusPolicy {
    bool reveal_hidden_followers;
    bool activate_editor_for_source_location;
    bool activate_chart_for_instrument;
    bool preserve_user_focus;
} UmiWorkbenchContextHostFocusPolicy;
/**
 * Provide the workbench context host focus policy default operation used by this module
 * and its client applications.
 */
UmiWorkbenchContextHostFocusPolicy umi_workbench_context_host_focus_policy_default(void);
/**
 * Provide the workbench context host focus decide operation used by this module and its
 * client applications.
 */
UmiWorkbenchContextHostFocusAction umi_workbench_context_host_focus_decide(
    const UmiWorkbenchContextHostFocusPolicy *policy,
    const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind context_kind,
    bool endpoint_visible,
    bool endpoint_active);
#ifdef __cplusplus
}
#endif
#endif
