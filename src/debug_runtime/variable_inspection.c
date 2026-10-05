/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/variable_inspection.c
 * PURPOSE: Keep nested debugger captures separate from root identity and reject stale actions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "variable_inspection_private.h"
#include "umicom/debug_runtime/variable_assignment.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
UmiStatus UmiDebugVariableTargetCapture(UmiDebugWorkspace *workspace, size_t index,
    UmiDebugVariableTarget **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugVariableTarget value = {0};
    UmiDebugVariableSnapshot variable;
    UmiDebugScopeSnapshot scope; UmiDebugStackFrameSnapshot frame;
    UmiDebugService *service = UmiDebugWorkspaceVariableService(workspace);
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &value.stamp);
    if (status == UMI_STATUS_OK) status = umi_debug_workspace_variable_at(workspace, index, &variable);
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_scope_registry_find(umi_debug_service_scope(service), variable.scope_id, &scope);
    if (status == UMI_STATUS_OK) status = umi_debug_stack_frame_registry_find(
        umi_debug_service_stack_frame(service), scope.frame_id, &frame);
    if (status == UMI_STATUS_OK) status = umi_debug_thread_registry_find(
        umi_debug_service_thread(service), frame.thread_id, &value.thread);
    if (status != UMI_STATUS_OK || !value.thread.stopped ||
        strcmp(scope.id, value.stamp.selectedScope) != 0 ||
        strcmp(frame.id, value.stamp.selectedFrame) != 0 ||
        strcmp(value.thread.id, value.stamp.selectedThread) != 0) return UMI_STATUS_INVALID_STATE;
    /* Canonical roots have smaller value/evaluate buffers. Copy the retained
     * record exactly; do not pretend it was freshly read by this operation. */
    memcpy(value.value.name, variable.name, sizeof variable.name);
    memcpy(value.value.value, variable.value, sizeof variable.value);
    memcpy(value.value.type, variable.type, sizeof variable.type);
    memcpy(value.value.evaluate_name, variable.evaluate_name, sizeof variable.evaluate_name);
    value.value.variables_reference = variable.variables_reference;
    /* DAP identifies assignments by parent reference and exact name. Refuse
     * duplicate names rather than choosing whichever registry row came first. */
    value.containerReference = scope.variables_reference;
    UmiDebugVariableRegistry *registry = umi_debug_service_variable(service);
    for (size_t i = 0U; i < umi_debug_variable_registry_count(registry); ++i) {
        UmiDebugVariableSnapshot sibling;
        status = umi_debug_variable_registry_at(registry, i, &sibling);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(sibling.id, variable.id) != 0 && strcmp(sibling.scope_id, variable.scope_id) == 0 &&
            strcmp(sibling.name, variable.name) == 0) value.ambiguousName = 1;
    }
    UmiDebugVariableTarget *target = malloc(sizeof *target);
    if (target == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *target = value; *out = target; return UMI_STATUS_OK;
}
void UmiDebugVariableTargetDestroy(UmiDebugVariableTarget *target) { free(target); }
UmiStatus UmiDebugVariableTargetRead(const UmiDebugVariableTarget *target, UmiDebugRuntimeVariable *out)
{
    if (target == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = target->value; return UMI_STATUS_OK;
}
UmiStatus UmiDebugVariableTargetValidate(UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target)
{
    if (workspace == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugViewStamp now;
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &now);
    if (status != UMI_STATUS_OK) return status;
    return UmiDebugViewStampEqual(&now, &target->stamp) ? UMI_STATUS_OK : UMI_STATUS_BUSY;
}
UmiStatus UmiDebugVariableTargetExpandable(const UmiDebugVariableTarget *target)
{
    if (target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (target->value.variables_reference == 0U) return UMI_STATUS_INVALID_STATE;
    if (target->value.variables_reference > INT32_MAX ||
        target->ancestorCount >= UMI_DEBUG_VARIABLE_INSPECTION_DEPTH) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < target->ancestorCount; ++i)
        if (target->ancestors[i] == target->value.variables_reference) return UMI_STATUS_ALREADY_EXISTS;
    return UMI_STATUS_OK;
}
static int Complete(const char *text, size_t capacity) { return memchr(text, '\0', capacity) != NULL; }
UmiStatus UmiDebugVariablePageCreate(const UmiDebugVariableTarget *target,
    const UmiDebugVariableChildren *children, UmiDebugVariablePage **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (target == NULL || children == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugVariableTargetExpandable(target);
    if (status != UMI_STATUS_OK) return status;
    if (children->count > UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < children->count; ++i) {
        const UmiDebugRuntimeVariable *v = &children->items[i];
        if (!Complete(v->name, sizeof v->name) || !Complete(v->value, sizeof v->value) ||
            !Complete(v->type, sizeof v->type) || !Complete(v->evaluate_name, sizeof v->evaluate_name) ||
            !Complete(v->memory_reference, sizeof v->memory_reference)) return UMI_STATUS_INVALID_ARGUMENT;
        if (v->variables_reference > INT32_MAX || v->named_variables > INT32_MAX || v->indexed_variables > INT32_MAX)
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiDebugVariablePage *page = malloc(sizeof *page + children->count * sizeof page->items[0]);
    if (page == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    page->parent = *target; page->count = children->count;
    memcpy(page->items, children->items, children->count * sizeof page->items[0]);
    *out = page; return UMI_STATUS_OK;
}
void UmiDebugVariablePageDestroy(UmiDebugVariablePage *page) { free(page); }
size_t UmiDebugVariablePageCount(const UmiDebugVariablePage *page) { return page != NULL ? page->count : 0U; }
UmiStatus UmiDebugVariablePageAt(const UmiDebugVariablePage *page, size_t index, UmiDebugRuntimeVariable *out)
{
    if (page == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= page->count) return UMI_STATUS_NOT_FOUND;
    *out = page->items[index]; return UMI_STATUS_OK;
}
UmiStatus UmiDebugVariablePageTarget(const UmiDebugVariablePage *page, size_t index, UmiDebugVariableTarget **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (page == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= page->count) return UMI_STATUS_NOT_FOUND;
    if (page->parent.ancestorCount >= UMI_DEBUG_VARIABLE_INSPECTION_DEPTH) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDebugVariableTarget *target = malloc(sizeof *target);
    if (target == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *target = page->parent;
    target->ancestors[target->ancestorCount++] = page->parent.value.variables_reference;
    /* Each nested row belongs to the expanded parent's container. Duplicate
     * display names remain inspectable but cannot become ambiguous edits. */
    target->containerReference = page->parent.value.variables_reference;
    target->ambiguousName = 0;
    for (size_t i = 0U; i < page->count; ++i)
        if (i != index && strcmp(page->items[i].name, page->items[index].name) == 0)
            target->ambiguousName = 1;
    target->value = page->items[index]; *out = target; return UMI_STATUS_OK;
}

UmiStatus UmiDebugVariableTargetAssignable(const UmiDebugVariableTarget *target)
{
    if (target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (target->containerReference == 0U || target->value.name[0] == '\0') return UMI_STATUS_INVALID_STATE;
    if (target->containerReference > INT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (target->ambiguousName) return UMI_STATUS_ALREADY_EXISTS;
    return UMI_STATUS_OK;
}
