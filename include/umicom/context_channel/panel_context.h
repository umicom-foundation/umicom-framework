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
typedef struct UmiPanelContextState { char panel_instance_id[UMI_CONTEXT_TEXT_CAPACITY]; char channel_id[UMI_CONTEXT_TEXT_CAPACITY]; UmiContextPayload current; bool has_context; bool following; uint64_t revision; } UmiPanelContextState;
void umi_panel_context_state_init(UmiPanelContextState *state,const char *panel_instance_id);
UmiStatus umi_panel_context_state_bind(UmiPanelContextState *state,const char *channel_id);
UmiStatus umi_panel_context_state_apply(UmiPanelContextState *state,const UmiContextPayload *payload);
void umi_panel_context_state_clear(UmiPanelContextState *state);
bool umi_panel_context_state_accepts(const UmiPanelContextState *state,const char *channel_id);
#ifdef __cplusplus
}
#endif
#endif
