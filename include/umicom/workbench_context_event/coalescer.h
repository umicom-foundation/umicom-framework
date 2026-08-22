/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/coalescer.h
 *
 * PURPOSE:
 *   Suppress duplicate high-frequency caret, selection and list-navigation events before context routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCER_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCER_H
#include "umicom/workbench_context_event/event.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextEventCoalescer {
    UmiWorkbenchContextEvent last;
    bool has_last;
    UmiWorkbenchContextEventCoalescingMode mode;
    uint64_t window_ms;
    uint64_t accepted_count;
    uint64_t coalesced_count;
    uint64_t revision;
} UmiWorkbenchContextEventCoalescer;
void umi_workbench_context_event_coalescer_init(
    UmiWorkbenchContextEventCoalescer *coalescer,
    UmiWorkbenchContextEventCoalescingMode mode,uint64_t window_ms);
bool umi_workbench_context_event_coalescer_accept(
    UmiWorkbenchContextEventCoalescer *coalescer,
    const UmiWorkbenchContextEvent *event);
#ifdef __cplusplus
}
#endif
#endif
