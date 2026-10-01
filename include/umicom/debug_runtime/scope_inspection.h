/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/scope_inspection.h
 * PURPOSE: Capture any scope in the selected stopped frame for explicit bounded inspection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_SCOPE_INSPECTION_H
#define UMICOM_DEBUG_RUNTIME_SCOPE_INSPECTION_H
#include "umicom/debug_runtime/variable_inspection.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Capture a scope by its current workspace ordering within the selected frame.
 * This includes expensive scopes and scopes other than selectedScope. No scope
 * selection, protocol request, registry write or variable evaluation occurs.
 * Normal workspace selection repair may occur before evidence is captured.
 *
 * The returned target owns its name/reference and full view stamp. Read returns
 * a synthetic container: scope name and reference, with empty value, type and
 * evaluation expression. Optional outScope receives the original scope record.
 * Its name is display text, never an identity or expression to evaluate.
 *
 * Use UmiDebugRuntimeInspectVariable explicitly to fetch this scope, and the
 * ordinary page/target APIs for nested values. Native liveness, pending events,
 * cycle checks, response bounds and no automatic retries remain unchanged.
 * A zero reference can be captured but is not expandable. A reference above
 * INT32_MAX can be read but Expandable rejects it. Expensive is a display hint,
 * not permission to run a request automatically.
 *
 * Owner-thread access only. Requires a retained stopped thread and matching
 * selected frame. Failure sets *outTarget to NULL and leaves outScope unchanged.
 * Copied data remains readable after workspace destruction. Any detail change
 * invalidates the target for new actions. Use UmiDebugVariableTargetDestroy.
 */
UmiStatus UmiDebugScopeInspectionCapture(UmiDebugWorkspace *workspace, size_t index,
    UmiDebugScopeSnapshot *outScope, UmiDebugVariableTarget **outTarget);
#ifdef __cplusplus
}
#endif
#endif
