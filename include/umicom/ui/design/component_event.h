/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_event.h
 *
 * PURPOSE:
 *   Define common semantic component events and bind them to stable Framework command identifiers.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_EVENT_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_EVENT_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignComponentEventKind { UMI_DESIGN_EVENT_ACTIVATE=1, UMI_DESIGN_EVENT_CHANGE=2, UMI_DESIGN_EVENT_SUBMIT=3, UMI_DESIGN_EVENT_SELECT=4, UMI_DESIGN_EVENT_EXPAND=5, UMI_DESIGN_EVENT_COLLAPSE=6, UMI_DESIGN_EVENT_CONTEXT=7 } UmiDesignComponentEventKind;
typedef struct UmiDesignComponentEvent { UmiDesignComponentEventKind kind; char command_id[UMI_DESIGN_ID_CAPACITY]; } UmiDesignComponentEvent;
/* Initialise a semantic event-to-command binding. */
UmiStatus umi_design_component_event_init(UmiDesignComponentEvent *event, UmiDesignComponentEventKind kind, const char *command_id);
/* Return a stable name for a semantic component event. */
const char *umi_design_component_event_name(UmiDesignComponentEventKind kind);

#ifdef __cplusplus
}
#endif

#endif
