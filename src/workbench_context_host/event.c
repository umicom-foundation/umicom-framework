/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/event.c
 *
 * PURPOSE:
 *   Initialise immutable context-host event records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/event.h"
#include <string.h>
/*
 * Initialise workbench context host event from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_host_event_init(UmiWorkbenchContextHostEvent *event,UmiWorkbenchContextHostEventKind kind)
{/* Apply this branch only when its contract condition is satisfied. */ if(event){memset(event,0,sizeof(*event));event->structure_size=(uint32_t)sizeof(*event);event->kind=kind;}}
