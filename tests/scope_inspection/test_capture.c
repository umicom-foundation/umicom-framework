/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/scope_inspection/test_capture.c
 * PURPOSE: Check scope capture identity, selection independence, lifetime and stale guards.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../variable_inspection/fixture.h"
#include "umicom/debug_runtime/scope_inspection.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    Fixture f = {0}; Open(&f); PopulateVariables(&f);
    UmiDebugScopeSnapshot global = {0}; strcpy(global.id, "global"); strcpy(global.frame_id, "0");
    strcpy(global.name, "Globals"); global.expensive = 1; global.variables_reference = 9U;
    OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &global));
    UmiDebugScopeSnapshot captured;
    UmiDebugVariableTarget *target = NULL;
    UmiDebugViewStamp before, after; OK(UmiDebugWorkspaceViewStamp(f.workspace, &before));
    OK(UmiDebugScopeInspectionCapture(f.workspace, 1U, &captured, &target));
    OK(UmiDebugWorkspaceViewStamp(f.workspace, &after)); CHECK(UmiDebugViewStampEqual(&before, &after));
    CHECK(strcmp(before.selectedScope, "scope") == 0 && strcmp(captured.id, "global") == 0);
    CHECK(captured.expensive && captured.variables_reference == 9U);
    UmiDebugRuntimeVariable value; OK(UmiDebugVariableTargetRead(target, &value));
    CHECK(strcmp(value.name, "Globals") == 0 && value.variables_reference == 9U);
    CHECK(value.value[0] == '\0' && value.type[0] == '\0' && value.evaluate_name[0] == '\0');
    OK(UmiDebugVariableTargetValidate(f.workspace, target));
    if (strcmp(name, "unselected") == 0) {
        UmiDebugVariableSnapshot root; OK(umi_debug_workspace_variable_at(f.workspace, 0U, &root));
        CHECK(strcmp(root.name, "notes") == 0);
    } else if (strcmp(name, "owned") == 0) {
        Close(&f); OK(UmiDebugVariableTargetRead(target, &value)); CHECK(strcmp(value.name, "Globals") == 0);
    } else if (strcmp(name, "bounds") == 0) {
        UmiDebugVariableTarget *missing = (UmiDebugVariableTarget *)1;
        UmiDebugScopeSnapshot sentinel = captured;
        CHECK(UmiDebugScopeInspectionCapture(f.workspace, 2U, &captured, &missing) == UMI_STATUS_NOT_FOUND && missing == NULL);
        CHECK(memcmp(&sentinel, &captured, sizeof captured) == 0);
        CHECK(UmiDebugScopeInspectionCapture(NULL, 0U, &captured, &missing) == UMI_STATUS_INVALID_ARGUMENT && missing == NULL);
        CHECK(UmiDebugScopeInspectionCapture(f.workspace, 0U, &captured, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "cheap") == 0) {
        UmiDebugVariableTarget *cheap = NULL; OK(UmiDebugScopeInspectionCapture(f.workspace, 0U, NULL, &cheap));
        OK(UmiDebugVariableTargetRead(cheap, &value)); CHECK(value.variables_reference == 1U);
        UmiDebugVariableTargetDestroy(cheap);
    } else if (strcmp(name, "name") == 0 || strcmp(name, "zero") == 0 || strcmp(name, "large") == 0) {
        if (strcmp(name, "name") == 0) { memset(global.name, 'a', sizeof global.name - 1U); global.name[254] = 'z'; }
        else global.variables_reference = strcmp(name, "zero") == 0 ? 0U : (uint64_t)INT32_MAX + 1U;
        OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &global));
        UmiDebugVariableTarget *next = NULL; OK(UmiDebugScopeInspectionCapture(f.workspace, 1U, NULL, &next));
        OK(UmiDebugVariableTargetRead(next, &value)); CHECK(strcmp(value.name, global.name) == 0);
        CHECK(UmiDebugVariableTargetExpandable(next) == (strcmp(name, "name") == 0 ? UMI_STATUS_OK :
            strcmp(name, "zero") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_CAPACITY_EXCEEDED));
        UmiDebugVariableTargetDestroy(next);
    } else if (strcmp(name, "foreign-frame") == 0) {
        strcpy(global.id, "other"); strcpy(global.frame_id, "other-frame");
        OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &global));
        UmiDebugVariableTarget *missing = NULL;
        CHECK(UmiDebugScopeInspectionCapture(f.workspace, 2U, NULL, &missing) == UMI_STATUS_NOT_FOUND && missing == NULL);
    } else if (strcmp(name, "owner") == 0) {
        Fixture other = {0}; Open(&other); PopulateVariables(&other);
        CHECK(UmiDebugVariableTargetValidate(other.workspace, target) == UMI_STATUS_BUSY); Close(&other);
    } else if (strcmp(name, "cycle") == 0 || strcmp(name, "child-stale") == 0) {
        UmiDebugVariableChildren *children = MakeChildren(strcmp(name, "cycle") == 0 ? 9U : 11U);
        UmiDebugVariablePage *page = NULL; UmiDebugVariableTarget *child = NULL;
        OK(UmiDebugVariablePageCreate(target, children, &page)); OK(UmiDebugVariablePageTarget(page, 0U, &child));
        if (strcmp(name, "cycle") == 0) CHECK(UmiDebugVariableTargetExpandable(child) == UMI_STATUS_ALREADY_EXISTS);
        else {
            OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &global));
            CHECK(UmiDebugVariableTargetValidate(f.workspace, child) == UMI_STATUS_BUSY);
        }
        UmiDebugVariableTargetDestroy(child); UmiDebugVariablePageDestroy(page); free(children);
    } else {
        if (strcmp(name, "running") == 0) {
            UmiDebugThreadSnapshot thread; OK(umi_debug_thread_registry_find(umi_debug_service_thread(f.service), "thread", &thread));
            thread.stopped = 0; OK(umi_debug_thread_registry_upsert(umi_debug_service_thread(f.service), &thread));
            UmiDebugVariableTarget *next = NULL;
            CHECK(UmiDebugScopeInspectionCapture(f.workspace, 1U, NULL, &next) == UMI_STATUS_INVALID_STATE && next == NULL);
        } else if (strcmp(name, "selection") == 0) OK(umi_debug_workspace_select_scope(f.workspace, "global"));
        else if (strcmp(name, "reuse") == 0 || strcmp(name, "scope-change") == 0) {
            if (strcmp(name, "reuse") == 0) OK(umi_debug_scope_registry_remove(umi_debug_service_scope(f.service), "global"));
            global.variables_reference = 10U;
            OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &global));
        } else {
            CHECK(strcmp(name, "root-change") == 0);
            UmiDebugVariableSnapshot root; OK(umi_debug_variable_registry_find(umi_debug_service_variable(f.service), "root", &root));
            strcpy(root.value, "new"); OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &root));
        }
        CHECK(UmiDebugVariableTargetValidate(f.workspace, target) == UMI_STATUS_BUSY);
    }
    UmiDebugVariableTargetDestroy(target); if (f.workspace != NULL) Close(&f); return 0;
}
