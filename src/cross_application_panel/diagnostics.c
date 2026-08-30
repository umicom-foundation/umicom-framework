/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/diagnostics.c
 *
 * PURPOSE:
 *   Implement reusable panel diagnostics service evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/diagnostics.h"
#include <string.h>
void umi_panel_diagnostics_state_init(UmiPanelDiagnosticsState *state){
if(state==NULL)return;
memset(state,0,sizeof(*state));
state->revision=1U;
state->last_status=UMI_STATUS_OK;
}
UmiStatus umi_panel_diagnostics_record(UmiPanelDiagnosticsState *state,UmiStatus status,const char *message){
UmiStatus copy;
if(state==NULL||message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
copy=umi_context_copy_text(state->last_message,sizeof(state->last_message),message);
if(copy!=UMI_STATUS_OK)return copy;
state->last_status=status;
state->operation_count+=1U;
state->revision+=1U;
return UMI_STATUS_OK;
}
bool umi_panel_diagnostics_healthy(const UmiPanelDiagnosticsState *state){
return state!=NULL&&state->last_status==UMI_STATUS_OK;
}
uint64_t umi_panel_diagnostics_operation_count(const UmiPanelDiagnosticsState *state){
return state==NULL?0U:state->operation_count;
}
