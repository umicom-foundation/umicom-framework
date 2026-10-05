/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/variable_assignment.h
 * PURPOSE: Assign a captured variable through an explicit, stopped-session operation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_VARIABLE_ASSIGNMENT_H
#define UMICOM_DEBUG_RUNTIME_VARIABLE_ASSIGNMENT_H
#include "umicom/debug_runtime/variable_inspection.h"
#ifdef __cplusplus
extern "C" {
#endif
/** An attempt can change the program even when its reply is lost. confirmed
 * means the adapter returned a complete success reply without queued events;
 * it is not an independent readback. value is populated only when confirmed.
 * The caller owns this result; keep it separate from all inputs. */
typedef struct UmiDebugVariableAssignment {
    int attempted;
    int confirmed;
    UmiDebugRuntimeEvaluateResult value;
} UmiDebugVariableAssignment;
/** Check the captured container/name without I/O. A duplicate sibling name
 * returns ALREADY_EXISTS because setVariable cannot identify it by row index.
 * Scalars can be assigned: their own child reference need not be nonzero. */
UmiStatus UmiDebugVariableTargetAssignable(const UmiDebugVariableTarget *target);
/** Check owner, current stopped frame, captured generations, adapter capability
 * and pending events. No request or mutation occurs. The result is a momentary
 * observation, not a reservation; AssignVariable repeats these checks. */
UmiStatus UmiDebugRuntimeCheckVariableAssignment(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target);
/** Send one explicit setVariable request for a captured root or child. value
 * is a NUL-terminated UTF-8 expression of at most 1023 bytes; the adapter decides
 * its language meaning and may run target code. An empty value is permitted.
 * Serialize all calls on the platform/workspace owner thread. timeout_ms > 0.
 * out is required and cleared on entry. Preflight failure has attempted=0.
 * Immediately before transport submission, discard variable registry captures
 * and advance the platform revision. This invalidates every previous target
 * and child page, even if submission, waiting or decoding fails. Existing owned
 * text remains readable. No assignment is retried, rolled back or fabricated.
 * After attempted=1, inspect the stopped session again before another edit.
 * A queued event yields BUSY and is retained for normal event processing.
 * This edits runtime state, not source files, watches or a saved configuration. */
UmiStatus UmiDebugRuntimeAssignVariable(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target,
    const char *value, uint32_t timeout_ms, UmiDebugVariableAssignment *out);
/** Strictly decode body.value (not evaluate's body.result) and optional type,
 * references and child counts. Recognized duplicates, invalid JSON/UTF-8,
 * truncation and non-integer/out-of-range references are refused. This decoder
 * validates a body; the transport caller must first check response success and
 * command correlation. Failure leaves out unchanged. */
UmiStatus UmiDebugRuntimeDecodeAssignmentValue(const char *json,
    UmiDebugRuntimeEvaluateResult *out);
#ifdef __cplusplus
}
#endif
#endif
