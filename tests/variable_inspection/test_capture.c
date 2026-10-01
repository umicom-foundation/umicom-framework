/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_inspection/test_capture.c
 * PURPOSE: Check root ownership, dependent selection, retained data and stale/reused reference guards.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; Open(&f); PopulateVariables(&f);
    UmiDebugVariableTarget *target = NULL; OK(UmiDebugVariableTargetCapture(f.workspace, 0U, &target));
    UmiDebugRuntimeVariable copied; OK(UmiDebugVariableTargetRead(target, &copied));
    CHECK(strcmp(copied.name, "notes") == 0 && copied.variables_reference == 2U);
    OK(UmiDebugVariableTargetValidate(f.workspace, target)); OK(UmiDebugVariableTargetExpandable(target));
    UmiDebugVariableRegistry *variables = umi_debug_service_variable(f.service);
    if (strcmp(name, "owned") == 0) {
        Close(&f); OK(UmiDebugVariableTargetRead(target, &copied)); CHECK(strcmp(copied.value, "{...}") == 0);
    } else if (strcmp(name, "bounds") == 0) {
        UmiDebugVariableTarget *missing = (UmiDebugVariableTarget *)1;
        CHECK(UmiDebugVariableTargetCapture(f.workspace, 1U, &missing) == UMI_STATUS_NOT_FOUND && missing == NULL);
        CHECK(UmiDebugVariableTargetCapture(NULL, 0U, &missing) == UMI_STATUS_INVALID_ARGUMENT && missing == NULL);
        CHECK(UmiDebugVariableTargetRead(NULL, &copied) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "owner") == 0) {
        Fixture other = {0}; Open(&other); PopulateVariables(&other);
        CHECK(UmiDebugVariableTargetValidate(other.workspace, target) == UMI_STATUS_BUSY); Close(&other);
    } else if (strcmp(name, "console") == 0) {
        UmiDebugConsoleEntrySnapshot entry = {0}; strcpy(entry.id, "output");
        OK(umi_debug_console_entry_registry_upsert(umi_debug_service_console_entry(f.service), &entry));
        OK(UmiDebugVariableTargetValidate(f.workspace, target));
    } else if (strcmp(name, "scalar") == 0 || strcmp(name, "large-reference") == 0) {
        UmiDebugVariableSnapshot variable; OK(umi_debug_variable_registry_find(variables, "root", &variable));
        variable.variables_reference = strcmp(name, "scalar") == 0 ? 0U : (uint64_t)INT32_MAX + 1U;
        OK(umi_debug_variable_registry_upsert(variables, &variable));
        UmiDebugVariableTarget *next = NULL; OK(UmiDebugVariableTargetCapture(f.workspace, 0U, &next));
        CHECK(UmiDebugVariableTargetExpandable(next) == (strcmp(name, "scalar") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_CAPACITY_EXCEEDED));
        UmiDebugVariableTargetDestroy(next);
    } else {
        if (strcmp(name, "variable") == 0 || strcmp(name, "reuse") == 0) {
            UmiDebugVariableSnapshot variable; OK(umi_debug_variable_registry_find(variables, "root", &variable));
            if (strcmp(name, "reuse") == 0) OK(umi_debug_variable_registry_remove(variables, "root"));
            else strcpy(variable.value, "changed");
            OK(umi_debug_variable_registry_upsert(variables, &variable));
        } else if (strcmp(name, "running") == 0) {
            UmiDebugThreadSnapshot thread; OK(umi_debug_thread_registry_find(umi_debug_service_thread(f.service), "thread", &thread));
            thread.stopped = 0; OK(umi_debug_thread_registry_upsert(umi_debug_service_thread(f.service), &thread));
            UmiDebugVariableTarget *next = NULL;
            CHECK(UmiDebugVariableTargetCapture(f.workspace, 0U, &next) == UMI_STATUS_INVALID_STATE && next == NULL);
        } else if (strcmp(name, "frame") == 0) {
            UmiDebugStackFrameSnapshot frame; OK(umi_debug_stack_frame_registry_find(umi_debug_service_stack_frame(f.service), "0", &frame));
            frame.line = 4; OK(umi_debug_stack_frame_registry_upsert(umi_debug_service_stack_frame(f.service), &frame));
        } else if (strcmp(name, "scope") == 0) {
            UmiDebugScopeSnapshot scope; OK(umi_debug_scope_registry_find(umi_debug_service_scope(f.service), "scope", &scope));
            strcpy(scope.frame_id, "missing"); OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &scope));
            UmiDebugVariableTarget *next = NULL;
            CHECK(UmiDebugVariableTargetCapture(f.workspace, 0U, &next) == UMI_STATUS_INVALID_STATE && next == NULL);
        } else {
            CHECK(strcmp(name, "session") == 0); UmiDebugSessionSnapshot session = {0}; strcpy(session.id, "session");
            OK(umi_debug_session_registry_upsert(umi_debug_service_session(f.service), &session));
        }
        CHECK(UmiDebugVariableTargetValidate(f.workspace, target) == UMI_STATUS_BUSY);
    }
    if (f.workspace != NULL) Close(&f);
    UmiDebugVariableTargetDestroy(target); return 0;
}
