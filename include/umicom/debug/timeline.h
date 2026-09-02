/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/timeline.h
 *
 * PURPOSE:
 *   Publish the public timeline contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_TIMELINE_H
#define UMICOM_DEBUG_TIMELINE_H
#include <stddef.h>
#include "umicom/debug/event.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DEBUG_TIMELINE_CAPACITY 1024U
/**
 * Represent the debug timeline data shared with callers of this public contract.
 */
typedef struct UmiDebugTimeline UmiDebugTimeline;
/**
 * Initialise debug timeline from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_timeline_create(UmiDebugTimeline **out_timeline);
/**
 * Release or reset state held by debug timeline so the same storage can be reused safely.
 */
void umi_debug_timeline_destroy(UmiDebugTimeline *timeline);
/**
 * Add debug timeline only after its inputs and available capacity have been checked.
 */
UmiStatus umi_debug_timeline_append(UmiDebugTimeline *timeline,const UmiDebugEventSnapshot *event);
/**
 * Find debug timeline while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_timeline_at(const UmiDebugTimeline *timeline,size_t index,UmiDebugEventSnapshot *out_event);
/**
 * Return the number of records represented by debug timeline without changing their state.
 */
size_t umi_debug_timeline_count(const UmiDebugTimeline *timeline);
/**
 * Release or reset state held by debug timeline so the same storage can be reused safely.
 */
void umi_debug_timeline_clear(UmiDebugTimeline *timeline);
#ifdef __cplusplus
}
#endif
#endif
