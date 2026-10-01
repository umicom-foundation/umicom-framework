/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/variable_inspection.h
 * PURPOSE: Own explicit debugger child captures without replacing the canonical scope variables.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_VARIABLE_INSPECTION_H
#define UMICOM_DEBUG_RUNTIME_VARIABLE_INSPECTION_H
#include "umicom/debug_runtime/platform.h"
#include "umicom/debug/selection.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY 128U
#define UMI_DEBUG_VARIABLE_INSPECTION_DEPTH 16U
typedef struct UmiDebugVariableTarget UmiDebugVariableTarget;
typedef struct UmiDebugVariablePage UmiDebugVariablePage;
/** An entire response, never a silently shortened prefix. Allocate on the
 * heap: variable values can be 4095 UTF-8 bytes each. Counts are adapter hints,
 * not proof that more children can be obtained through paging. */
typedef struct UmiDebugVariableChildren {
    size_t count;
    UmiDebugRuntimeVariable items[UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY];
} UmiDebugVariableChildren;
/** Copy a root variable in current visible ordering. Requires its retained
 * stopped thread, selected frame and scope to agree. No adapter request occurs.
 * All owned targets/pages survive workspace destruction for reading only.
 * Every owning output is NULL on failure. Destroy each successful output. */
UmiStatus UmiDebugVariableTargetCapture(UmiDebugWorkspace *workspace, size_t index,
    UmiDebugVariableTarget **out);
void UmiDebugVariableTargetDestroy(UmiDebugVariableTarget *target);
UmiStatus UmiDebugVariableTargetRead(const UmiDebugVariableTarget *target,
    UmiDebugRuntimeVariable *out);
/** Check owner, selection and every debugger detail generation. BUSY means
 * recapture from current roots; a reused numeric reference is not the old object.
 * Console output alone does not invalidate captures. Serialize owner-thread use. */
UmiStatus UmiDebugVariableTargetValidate(UmiDebugWorkspace *workspace,
    const UmiDebugVariableTarget *target);
/** OK for a bounded nonzero reference with no ancestor cycle. A scalar returns
 * INVALID_STATE; cycles return ALREADY_EXISTS; excessive depth/reference returns
 * CAPACITY_EXCEEDED. This does not validate workspace or process liveness. */
UmiStatus UmiDebugVariableTargetExpandable(const UmiDebugVariableTarget *target);
/** Strict complete child decoder. Requires body.variables array; each row needs
 * string name/value and a nonnegative integer variablesReference. Optional type,
 * evaluateName, memoryReference and counts must have the proper type when present.
 * Required/recognized duplicate members (including escaped names), invalid UTF-8,
 * embedded NUL, invalid integer values and capacity excess fail atomically.
 * Repeated display names are valid and retain distinct row positions.
 * out remains unchanged on failure. Unknown bounded member names are ignored. */
UmiStatus UmiDebugRuntimeDecodeVariableChildren(const char *json,
    UmiDebugVariableChildren *out);
/** Bind a complete response to a copied target. Intended for adapter hosts which
 * have independently checked stopped-state and request completion. It performs
 * bounded text/count validation, not protocol I/O or process-liveness validation.
 * Inputs are copied. The native helper below owns all execution checks. */
UmiStatus UmiDebugVariablePageCreate(const UmiDebugVariableTarget *target,
    const UmiDebugVariableChildren *children, UmiDebugVariablePage **out);
void UmiDebugVariablePageDestroy(UmiDebugVariablePage *page);
size_t UmiDebugVariablePageCount(const UmiDebugVariablePage *page);
UmiStatus UmiDebugVariablePageAt(const UmiDebugVariablePage *page, size_t index,
    UmiDebugRuntimeVariable *out);
/** Capture a child by position, preserving the parent's workspace stamp and
 * reference ancestry. Equal names do not collide. Scalars can be read; use
 * Expandable before requesting another level. No request or mutation occurs. */
UmiStatus UmiDebugVariablePageTarget(const UmiDebugVariablePage *page, size_t index,
    UmiDebugVariableTarget **out);
/** Explicitly request one whole child list from an active, inspected, stopped
 * native session owned by workspace. Requires timeout_ms > 0. Queued events
 * before/after waiting yield BUSY and never publish a page. Events stay queued.
 * No registry is overwritten, expression evaluated, variable assigned or process
 * launched. Adapter visualizers may themselves execute target code. Failures
 * are not automatically retried. Each page is a last capture, not a live view. */
UmiStatus UmiDebugRuntimeInspectVariable(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target,
    uint32_t timeout_ms, UmiDebugVariablePage **out);
#ifdef __cplusplus
}
#endif
#endif
