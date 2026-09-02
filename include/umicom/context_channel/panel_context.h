/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/panel_context.h
 *
 * PURPOSE:
 *   Hold the current context consumed by one panel instance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PANEL_CONTEXT_H
#define UMICOM_CONTEXT_CHANNEL_PANEL_CONTEXT_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel context state data shared with callers of this public contract.
 */
typedef struct UmiPanelContextState { char panel_instance_id[UMI_CONTEXT_TEXT_CAPACITY]; char channel_id[UMI_CONTEXT_TEXT_CAPACITY]; UmiContextPayload current; bool has_context; bool following; uint64_t revision; } UmiPanelContextState;
/**
 * Initialise panel context state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_context_state_init(UmiPanelContextState *state,const char *panel_instance_id);
/**
 * Provide the panel context state bind operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_context_state_bind(UmiPanelContextState *state,const char *channel_id);
/**
 * Perform panel context state through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_panel_context_state_apply(UmiPanelContextState *state,const UmiContextPayload *payload);
/**
 * Release or reset state held by panel context state so the same storage can be reused
 * safely.
 */
void umi_panel_context_state_clear(UmiPanelContextState *state);
/**
 * Provide the panel context state accepts operation used by this module and its client
 * applications.
 */
bool umi_panel_context_state_accepts(const UmiPanelContextState *state,const char *channel_id);
#ifdef __cplusplus
}
#endif
#endif
