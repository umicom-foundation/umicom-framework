/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_assignment/test_platform.c
 * PURPOSE: Exercise real framed assignment, capability refusal and one-attempt capture invalidation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../variable_inspection/fixture.h"
#include "umicom/debug_runtime/variable_assignment.h"
static void StopAtFixture(UmiDebugRuntimePlatform *platform)
{
    for (unsigned i = 0U; i < 80U; ++i) {
        UmiDebugRuntimePlatformSnapshot snapshot; OK(umi_debug_runtime_platform_snapshot(platform, &snapshot));
        if (snapshot.paused) return;
        int handled = 0; UmiStatus status = umi_debug_runtime_platform_pump_event(platform, 25U, &handled);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    CHECK(argc == 3); const char *name = argv[2]; UmiDebugRuntimePlatform *platform = NULL;
    OK(umi_debug_runtime_platform_create(&platform)); Fixture f = {0}; OpenWithService(&f, umi_debug_runtime_platform_service(platform));
    UmiDebugVariableTarget *target = NULL, *child = NULL; UmiDebugVariablePage *page = NULL;
    if (strcmp(name, "idle") == 0) {
        PopulateVariables(&f); OK(UmiDebugVariableTargetCapture(f.workspace, 0U, &target));
        UmiDebugVariableAssignment result;
        CHECK(UmiDebugRuntimeAssignVariable(platform, f.workspace, target, "7", 100U, &result) == UMI_STATUS_INVALID_STATE);
        CHECK(!result.attempted && !result.confirmed);
        UmiDebugVariableTargetDestroy(target); Close(&f); umi_debug_runtime_platform_destroy(platform); return 0;
    }
    UmiDebugAdapterProfile profile = *umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
    (void)snprintf(profile.executable, sizeof profile.executable, "%s", argv[1]);
    (void)snprintf(profile.arguments, sizeof profile.arguments, "variable-assignment-%s", name);
    OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(f.service), &profile));
    OK(umi_debug_runtime_platform_start(platform, profile.id, "session", "launch", "{}", 0, NULL, 2000U));
    StopAtFixture(platform); OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
    if (strcmp(name, "duplicate") == 0) {
        UmiDebugVariableSnapshot duplicate; OK(umi_debug_workspace_variable_at(f.workspace, 0U, &duplicate));
        strcpy(duplicate.id, "duplicate"); OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &duplicate));
    }
    OK(UmiDebugVariableTargetCapture(f.workspace, 0U, &target));
    if (strcmp(name, "prequeued") == 0) {
        CHECK(UmiDebugRuntimeInspectVariable(platform, f.workspace, target, 2000U, &page) == UMI_STATUS_BUSY);
        CHECK(page == NULL);
    }
    if (strcmp(name, "nested") == 0) {
        OK(UmiDebugRuntimeInspectVariable(platform, f.workspace, target, 2000U, &page));
        OK(UmiDebugVariablePageTarget(page, 0U, &child));
    }
    if (strcmp(name, "stale") == 0) {
        UmiDebugVariableSnapshot root; OK(umi_debug_workspace_variable_at(f.workspace, 0U, &root));
        OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &root));
    }
    char large[1025]; memset(large, 'x', sizeof large - 1U); large[sizeof large - 1U] = '\0';
    char boundary[1024]; memset(boundary, '\t', sizeof boundary - 1U); boundary[sizeof boundary - 1U] = '\0';
    const char *value = strcmp(name, "unicode") == 0 ? "caf\xc3\xa9 \\\"" :
        strcmp(name, "empty") == 0 ? "" : strcmp(name, "long-input") == 0 ? large :
        strcmp(name, "boundary-input") == 0 ? boundary : strcmp(name, "invalid-utf8") == 0 ? "\xc0\xaf" : "7";
    UmiDebugRuntimePlatformSnapshot before, after; OK(umi_debug_runtime_platform_snapshot(platform, &before));
    UmiDebugVariableAssignment result; UmiStatus status;
    if (strcmp(name, "legacy-variable") == 0 || strcmp(name, "legacy-expression") == 0) {
        UmiDebugRuntimeEvaluateResult reply;
        status = strcmp(name, "legacy-variable") == 0 ? umi_debug_runtime_platform_set_variable(platform, 1U, "notes", value, 2000U, &reply) :
            umi_debug_runtime_platform_set_expression(platform, "notes", value, 0U, 2000U, &reply);
        CHECK(status == UMI_STATUS_OK && strcmp(reply.result, value) == 0);
        CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(f.service)) == 0U);
        CHECK(UmiDebugVariableTargetValidate(f.workspace, target) == UMI_STATUS_BUSY);
    } else {
        Fixture other = {0}; UmiDebugWorkspace *workspace = f.workspace;
        if (strcmp(name, "owner") == 0) { Open(&other); PopulateVariables(&other); workspace = other.workspace; }
        status = UmiDebugRuntimeAssignVariable(platform, workspace, child != NULL ? child : target, value,
            strcmp(name, "timeout") == 0 ? 80U : strcmp(name, "zero-timeout") == 0 ? 0U : 2000U, &result);
        if (other.workspace != NULL) Close(&other);
        OK(umi_debug_runtime_platform_snapshot(platform, &after));
        int preflight = strcmp(name, "unsupported") == 0 || strcmp(name, "stale") == 0 || strcmp(name, "owner") == 0 ||
            strcmp(name, "zero-timeout") == 0 || strcmp(name, "duplicate") == 0 || strcmp(name, "prequeued") == 0 || strcmp(name, "long-input") == 0 || strcmp(name, "invalid-utf8") == 0;
        if (preflight) {
            CHECK(!result.attempted && !result.confirmed && before.adapter.messages_sent == after.adapter.messages_sent);
            CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(f.service)) == (strcmp(name, "duplicate") == 0 ? 2U : 1U));
            UmiStatus expected = strcmp(name, "unsupported") == 0 ? UMI_STATUS_NOT_IMPLEMENTED :
                (strcmp(name, "stale") == 0 || strcmp(name, "prequeued") == 0) ? UMI_STATUS_BUSY :
                strcmp(name, "duplicate") == 0 ? UMI_STATUS_ALREADY_EXISTS : strcmp(name, "long-input") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED :
                strcmp(name, "invalid-utf8") == 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_INVALID_ARGUMENT;
            CHECK(status == expected);
            if (strcmp(name, "prequeued") == 0) CHECK(after.adapter.queued_events == 1U);
        } else {
            CHECK(result.attempted && after.adapter.messages_sent == before.adapter.messages_sent + 1U);
            CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(f.service)) == 0U);
            CHECK(UmiDebugVariableTargetValidate(f.workspace, target) == UMI_STATUS_BUSY);
            UmiDebugVariableAssignment repeated;
            CHECK(UmiDebugRuntimeAssignVariable(platform, f.workspace, child != NULL ? child : target, value, 500U, &repeated) == UMI_STATUS_BUSY);
            CHECK(!repeated.attempted); UmiDebugRuntimePlatformSnapshot blocked; OK(umi_debug_runtime_platform_snapshot(platform, &blocked));
            CHECK(blocked.adapter.messages_sent == after.adapter.messages_sent);
            if (strcmp(name, "rejected") == 0) CHECK(status == UMI_STATUS_UNAVAILABLE && !result.confirmed);
            else if (strcmp(name, "timeout") == 0) CHECK(status == UMI_STATUS_TIMEOUT && !result.confirmed);
            else if (strcmp(name, "event") == 0) CHECK(status == UMI_STATUS_BUSY && !result.confirmed && after.adapter.queued_events == 1U);
            else if (strcmp(name, "command") == 0 || strcmp(name, "malformed") == 0) CHECK(status == UMI_STATUS_PARSE_ERROR && !result.confirmed);
            else {
                CHECK(status == UMI_STATUS_OK && result.confirmed && strcmp(result.value.result, value) == 0);
                /* A separate later read confirms the peer's changed state. */
                OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
                UmiDebugVariableSnapshot root; OK(umi_debug_workspace_variable_at(f.workspace, 0U, &root));
                CHECK(strcmp(root.value, value) == 0);
            }
        }
    }
    UmiDebugVariableTargetDestroy(child); UmiDebugVariablePageDestroy(page); UmiDebugVariableTargetDestroy(target);
    OK(umi_debug_runtime_platform_stop(platform, 1, 2000U)); Close(&f); umi_debug_runtime_platform_destroy(platform); return 0;
}
