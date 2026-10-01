/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/scope_inspection.c
 * PURPOSE: Reuse owned variable requests for unselected and expensive scope containers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/scope_inspection.h"
#include "variable_inspection_private.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiDebugScopeInspectionCapture(UmiDebugWorkspace *workspace, size_t index,
    UmiDebugScopeSnapshot *outScope, UmiDebugVariableTarget **outTarget)
{
    if (outTarget == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outTarget = NULL;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugVariableTarget value = {0};
    UmiDebugScopeSnapshot scope;
    UmiDebugStackFrameSnapshot frame;
    UmiDebugService *service = UmiDebugWorkspaceVariableService(workspace);
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &value.stamp);
    if (status == UMI_STATUS_OK) status = umi_debug_workspace_scope_at(workspace, index, &scope);
    if (status == UMI_STATUS_OK) status = umi_debug_stack_frame_registry_find(
        umi_debug_service_stack_frame(service), scope.frame_id, &frame);
    if (status == UMI_STATUS_OK) status = umi_debug_thread_registry_find(
        umi_debug_service_thread(service), frame.thread_id, &value.thread);
    if (status != UMI_STATUS_OK) return status;
    if (!value.thread.stopped || strcmp(scope.frame_id, value.stamp.selectedFrame) != 0 ||
        strcmp(frame.id, value.stamp.selectedFrame) != 0 ||
        strcmp(value.thread.id, value.stamp.selectedThread) != 0) return UMI_STATUS_INVALID_STATE;
    /* A scope is a container, not an evaluated variable. Framework retains its
     * reference under the same stop evidence as ordinary compound variables;
     * applications never synthesize references or replace canonical roots. */
    _Static_assert(sizeof value.value.name >= sizeof scope.name, "Scope names must fit completely");
    memcpy(value.value.name, scope.name, sizeof scope.name);
    value.value.variables_reference = scope.variables_reference;
    UmiDebugVariableTarget *target = malloc(sizeof *target);
    if (target == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *target = value;
    if (outScope != NULL) *outScope = scope;
    *outTarget = target;
    return UMI_STATUS_OK;
}
