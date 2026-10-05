/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_assignment/test_target.c
 * PURPOSE: Verify parent-container identity and refuse ambiguous assignment targets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../variable_inspection/fixture.h"
#include "umicom/debug_runtime/variable_assignment.h"
#include "umicom/debug_runtime/scope_inspection.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; Open(&f); PopulateVariables(&f);
    UmiDebugVariableSnapshot variable; OK(umi_debug_workspace_variable_at(f.workspace, 0U, &variable));
    UmiDebugScopeSnapshot scope; OK(umi_debug_workspace_scope_at(f.workspace, 0U, &scope));
    if (strcmp(name, "scalar") == 0) variable.variables_reference = 0U;
    if (strcmp(name, "empty-name") == 0) variable.name[0] = '\0';
    if (strcmp(name, "missing-container") == 0) scope.variables_reference = 0U;
    if (strcmp(name, "large-container") == 0) scope.variables_reference = (uint64_t)INT32_MAX + 1U;
    OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &variable));
    OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &scope));
    if (strcmp(name, "duplicate-root") == 0) {
        strcpy(variable.id, "another"); OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &variable));
    }
    UmiDebugVariableTarget *target = NULL; OK(UmiDebugVariableTargetCapture(f.workspace, 0U, &target));
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "missing-container") == 0 || strcmp(name, "empty-name") == 0) expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(name, "large-container") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(name, "duplicate-root") == 0) expected = UMI_STATUS_ALREADY_EXISTS;
    CHECK(UmiDebugVariableTargetAssignable(target) == expected);
    if (strcmp(name, "nested") == 0 || strcmp(name, "duplicate-child") == 0) {
        UmiDebugVariableChildren *children = MakeChildren(0U);
        if (strcmp(name, "duplicate-child") == 0) { children->count = 2U; children->items[1] = children->items[0]; }
        UmiDebugVariablePage *page = NULL; UmiDebugVariableTarget *child = NULL;
        OK(UmiDebugVariablePageCreate(target, children, &page)); OK(UmiDebugVariablePageTarget(page, 0U, &child));
        CHECK(UmiDebugVariableTargetAssignable(child) == (children->count == 2U ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_OK));
        UmiDebugVariablePageDestroy(page); free(children);
        /* The child remains an owned value after its source page is released. */
        UmiDebugRuntimeVariable value; OK(UmiDebugVariableTargetRead(child, &value)); CHECK(strcmp(value.name, "item") == 0);
        UmiDebugVariableTargetDestroy(child);
    } else if (strcmp(name, "scope") == 0) {
        UmiDebugVariableTarget *container = NULL; OK(UmiDebugScopeInspectionCapture(f.workspace, 0U, NULL, &container));
        CHECK(UmiDebugVariableTargetAssignable(container) == UMI_STATUS_INVALID_STATE); UmiDebugVariableTargetDestroy(container);
    } else if (strcmp(name, "invalidate") == 0) {
        umi_debug_variable_registry_clear(umi_debug_service_variable(f.service));
        CHECK(UmiDebugVariableTargetValidate(f.workspace, target) == UMI_STATUS_BUSY);
        UmiDebugRuntimeVariable value; OK(UmiDebugVariableTargetRead(target, &value)); CHECK(strcmp(value.name, "notes") == 0);
    } else CHECK(strcmp(name, "root") == 0 || strcmp(name, "scalar") == 0 || strcmp(name, "empty-name") == 0 ||
        strcmp(name, "missing-container") == 0 || strcmp(name, "large-container") == 0 || strcmp(name, "duplicate-root") == 0);
    UmiDebugVariableTargetDestroy(target); Close(&f); return 0;
}
