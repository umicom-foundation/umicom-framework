/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/coalescer.h
 *
 * PURPOSE:
 *   Suppress duplicate high-frequency caret, selection and list-navigation events before context routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCER_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCER_H
#include "umicom/workbench_context_event/event.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context event coalescer data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventCoalescer {
    UmiWorkbenchContextEvent last;
    bool has_last;
    UmiWorkbenchContextEventCoalescingMode mode;
    uint64_t window_ms;
    uint64_t accepted_count;
    uint64_t coalesced_count;
    uint64_t revision;
} UmiWorkbenchContextEventCoalescer;
/**
 * Initialise workbench context event coalescer from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_coalescer_init(
    UmiWorkbenchContextEventCoalescer *coalescer,
    UmiWorkbenchContextEventCoalescingMode mode,uint64_t window_ms);
/**
 * Provide the workbench context event coalescer accept operation used by this module and
 * its client applications.
 */
bool umi_workbench_context_event_coalescer_accept(
    UmiWorkbenchContextEventCoalescer *coalescer,
    const UmiWorkbenchContextEvent *event);
#ifdef __cplusplus
}
#endif
#endif
