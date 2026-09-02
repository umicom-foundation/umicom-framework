/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_event.c
 *
 * PURPOSE:
 *   Define common semantic component events and bind them to stable Framework command identifiers.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_event.h"

#include <string.h>
/*
 * Initialise design component event from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_component_event_init(UmiDesignComponentEvent *event,UmiDesignComponentEventKind kind,const char *command_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(event==NULL||command_id==NULL||kind<UMI_DESIGN_EVENT_ACTIVATE||kind>UMI_DESIGN_EVENT_CONTEXT)return UMI_STATUS_INVALID_ARGUMENT;memset(event,0,sizeof *event);event->kind=kind;return umi_design_copy_text(event->command_id,sizeof event->command_id,command_id);}
/*
 * Provide the design component event name operation used by this module and its client
 * applications.
 */
const char *umi_design_component_event_name(UmiDesignComponentEventKind kind){/* Select the behaviour associated with the requested command or state value. */ switch(kind){case UMI_DESIGN_EVENT_ACTIVATE:return "activate";case UMI_DESIGN_EVENT_CHANGE:return "change";case UMI_DESIGN_EVENT_SUBMIT:return "submit";case UMI_DESIGN_EVENT_SELECT:return "select";case UMI_DESIGN_EVENT_EXPAND:return "expand";case UMI_DESIGN_EVENT_COLLAPSE:return "collapse";case UMI_DESIGN_EVENT_CONTEXT:return "context";default:return "unknown";}}
