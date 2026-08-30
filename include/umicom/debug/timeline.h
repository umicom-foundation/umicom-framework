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
typedef struct UmiDebugTimeline UmiDebugTimeline;
UmiStatus umi_debug_timeline_create(UmiDebugTimeline **out_timeline);
void umi_debug_timeline_destroy(UmiDebugTimeline *timeline);
UmiStatus umi_debug_timeline_append(UmiDebugTimeline *timeline,const UmiDebugEventSnapshot *event);
UmiStatus umi_debug_timeline_at(const UmiDebugTimeline *timeline,size_t index,UmiDebugEventSnapshot *out_event);
size_t umi_debug_timeline_count(const UmiDebugTimeline *timeline);
void umi_debug_timeline_clear(UmiDebugTimeline *timeline);
#ifdef __cplusplus
}
#endif
#endif
