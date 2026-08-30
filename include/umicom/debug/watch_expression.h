/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/watch_expression.h
 *
 * PURPOSE:
 *   Publish the public watch expression contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WATCH_EXPRESSION_H
#define UMICOM_DEBUG_WATCH_EXPRESSION_H
#include "umicom/debug/watch.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugWatchEvaluation{char watch_id[128];char expression[1024];char context[64];int frame_id;int enabled;uint64_t revision;}UmiDebugWatchEvaluation;
UmiStatus umi_debug_watch_evaluation_from_snapshot(const UmiDebugWatchSnapshot *watch,int frame_id,UmiDebugWatchEvaluation *out_evaluation);
#ifdef __cplusplus
}
#endif
#endif
