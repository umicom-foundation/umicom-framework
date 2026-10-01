/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/watch_evaluation.h
 * PURPOSE: Describe explicit watch evaluation and complete bounded response values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_WATCH_EVALUATION_H
#define UMICOM_DEBUG_RUNTIME_WATCH_EVALUATION_H
#include "umicom/debug_runtime/platform.h"
#include "umicom/debug/watch_edit.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugWatchValue { char value[1024], type[256]; } UmiDebugWatchValue;
/** Decode the required string body.result and optional string body.type.
 * Empty result is valid. Missing/wrong fields and ambiguous duplicate members
 * fail; values must fit completely. Failure leaves out unchanged. */
UmiStatus UmiDebugRuntimeDecodeWatchValue(const char *json, UmiDebugWatchValue *out);
/** Explicitly evaluate an enabled captured watch in the currently selected
 * native frame. The workspace must own this platform's service. Requires an
 * active paused process and an inspected retained frame, including frame zero.
 * No launch, automatic retry or expression editing occurs. Evaluation can run
 * target code. A failed/uncertain request must be reviewed before another click.
 *
 * Pending adapter events before dispatch return BUSY. Events received while
 * waiting also return BUSY without replacing the previous value; they remain
 * queued for ordinary event processing. This conservative rule includes output
 * events. A successful result is a last explicit capture, not a live value.
 * Owner-thread access only. timeout_ms must be nonzero.
 */
UmiStatus UmiDebugRuntimeEvaluateWatchEdit(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit, uint32_t timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
