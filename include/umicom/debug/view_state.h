/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/view_state.h
 *
 * PURPOSE:
 *   Publish the public view state contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_VIEW_STATE_H
#define UMICOM_DEBUG_VIEW_STATE_H
#include "umicom/debug/service.h"
#include "umicom/debug/controller.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugViewState{char state[64];size_t configurations;size_t breakpoints;size_t sessions;size_t threads;size_t frames;size_t scopes;size_t variables;size_t watches;size_t console_entries;size_t modules;size_t sources;size_t exceptions;size_t events;int active_thread_id;int active_frame_id;uint64_t revision;}UmiDebugViewState;
UmiStatus umi_debug_view_state_build(const UmiDebugService *service,const UmiDebugController *controller,UmiDebugViewState *out_state);
#ifdef __cplusplus
}
#endif
#endif
