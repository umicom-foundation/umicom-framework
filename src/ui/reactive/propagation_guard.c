/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/propagation_guard.c
 *
 * PURPOSE:
 *   Bound propagation depth to prevent runaway reactive graphs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_guard.h"
/* Enter one propagation frame and reject recursion beyond the configured limit. */
UmiStatus umi_ui_reactive_propagation_guard_enter(UmiUiReactivePropagationGuard *g){if(!g||g->max_depth==0U)return UMI_STATUS_INVALID_ARGUMENT;if(g->depth>=g->max_depth){g->blocked=true;return UMI_STATUS_CAPACITY_EXCEEDED;}g->depth++;return UMI_STATUS_OK;}
/* Leave one propagation frame without underflow. */
void umi_ui_reactive_propagation_guard_leave(UmiUiReactivePropagationGuard *g){if(g&&g->depth>0U)g->depth--;}
