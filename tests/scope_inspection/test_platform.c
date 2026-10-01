/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/scope_inspection/test_platform.c
 * PURPOSE: Verify expensive scope requests never replace roots or bypass native stop and event guards.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../variable_inspection/fixture.h"
#include "umicom/debug_runtime/scope_inspection.h"
int main(int argc, char **argv)
{
    CHECK(argc == 3); const char *name = argv[2];
    UmiDebugRuntimePlatform *platform = NULL; OK(umi_debug_runtime_platform_create(&platform));
    Fixture f = {0}; OpenWithService(&f, umi_debug_runtime_platform_service(platform));
    UmiDebugAdapterProfile profile = *umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
    (void)snprintf(profile.executable, sizeof profile.executable, "%s", argv[1]);
    (void)snprintf(profile.arguments, sizeof profile.arguments, "scope-inspection-%s", name);
    OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(f.service), &profile));
    OK(umi_debug_runtime_platform_start(platform, profile.id, "session", "launch", "{}", 0, NULL, 2000U));
    UmiDebugRuntimePlatformSnapshot state;
    for (unsigned i = 0U; i < 80U; ++i) {
        OK(umi_debug_runtime_platform_snapshot(platform, &state)); if (state.paused) break;
        int handled = 0; UmiStatus status = umi_debug_runtime_platform_pump_event(platform, 25U, &handled);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
    }
    CHECK(state.paused); OK(umi_debug_runtime_platform_snapshot(platform, &state));
    OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
    UmiDebugRuntimePlatformSnapshot inspected; OK(umi_debug_runtime_platform_snapshot(platform, &inspected));
    int only = strcmp(name, "only-expensive") == 0;
    CHECK(inspected.adapter.messages_sent == state.adapter.messages_sent + (only ? 3U : 4U));
    CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(f.service)) == (only ? 0U : 1U));
    UmiDebugScopeSnapshot scope; UmiDebugVariableTarget *target = NULL;
    OK(UmiDebugScopeInspectionCapture(f.workspace, only ? 0U : 1U, &scope, &target));
    CHECK(scope.expensive && scope.variables_reference == 9U);
    if (strcmp(name, "stale") == 0) OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f.service), &scope));
    UmiDebugViewStamp before, after; OK(UmiDebugWorkspaceViewStamp(f.workspace, &before));
    UmiDebugVariablePage *page = NULL;
    UmiStatus status = UmiDebugRuntimeInspectVariable(platform, f.workspace, target,
        strcmp(name, "timeout") == 0 ? 80U : 2000U, &page);
    OK(UmiDebugWorkspaceViewStamp(f.workspace, &after)); CHECK(UmiDebugViewStampEqual(&before, &after));
    CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(f.service)) == (only ? 0U : 1U));
    if (strcmp(name, "stale") == 0 || strcmp(name, "event") == 0 || strcmp(name, "timeout") == 0 ||
        strcmp(name, "malformed") == 0 || strcmp(name, "rejected") == 0) {
        UmiStatus expected = strcmp(name, "stale") == 0 || strcmp(name, "event") == 0 ? UMI_STATUS_BUSY :
            strcmp(name, "timeout") == 0 ? UMI_STATUS_TIMEOUT :
            strcmp(name, "rejected") == 0 ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_PARSE_ERROR;
        CHECK(status == expected && page == NULL);
        OK(umi_debug_runtime_platform_snapshot(platform, &state));
        if (strcmp(name, "stale") == 0) CHECK(state.adapter.messages_sent == inspected.adapter.messages_sent);
        if (strcmp(name, "event") == 0) {
            CHECK(state.adapter.queued_events == 1U);
            CHECK(UmiDebugRuntimeInspectVariable(platform, f.workspace, target, 2000U, &page) == UMI_STATUS_BUSY);
            UmiDebugRuntimePlatformSnapshot blocked; OK(umi_debug_runtime_platform_snapshot(platform, &blocked));
            CHECK(blocked.adapter.messages_sent == state.adapter.messages_sent && blocked.adapter.queued_events == 1U);
        }
    } else {
        CHECK(status == UMI_STATUS_OK && page != NULL);
        if (strcmp(name, "empty") == 0) CHECK(UmiDebugVariablePageCount(page) == 0U);
        else {
            UmiDebugRuntimeVariable value; OK(UmiDebugVariablePageAt(page, 0U, &value));
            if (strcmp(name, "cycle") == 0 || strcmp(name, "nested") == 0) {
                UmiDebugVariableTarget *child = NULL; UmiDebugVariablePage *nested = NULL;
                OK(UmiDebugVariablePageTarget(page, 0U, &child));
                UmiStatus result = UmiDebugRuntimeInspectVariable(platform, f.workspace, child, 2000U, &nested);
                if (strcmp(name, "cycle") == 0) CHECK(result == UMI_STATUS_ALREADY_EXISTS && nested == NULL);
                else { CHECK(result == UMI_STATUS_OK); OK(UmiDebugVariablePageAt(nested, 0U, &value)); CHECK(strcmp(value.value, "7") == 0); }
                UmiDebugVariableTargetDestroy(child); UmiDebugVariablePageDestroy(nested);
            } else {
                CHECK(strcmp(value.value, "capture 1") == 0); /* Expensive scope was not prefetched. */
                if (strcmp(name, "repeat") == 0) {
                    UmiDebugVariablePage *again = NULL;
                    OK(UmiDebugRuntimeInspectVariable(platform, f.workspace, target, 2000U, &again));
                    OK(UmiDebugVariablePageAt(again, 0U, &value)); CHECK(strcmp(value.value, "capture 2") == 0);
                    OK(UmiDebugVariablePageAt(page, 0U, &value)); CHECK(strcmp(value.value, "capture 1") == 0);
                    UmiDebugVariablePageDestroy(again);
                } else CHECK(strcmp(name, "success") == 0 || only);
            }
        }
    }
    OK(umi_debug_runtime_platform_stop(platform, 1, 2000U)); Close(&f); umi_debug_runtime_platform_destroy(platform);
    if (page != NULL && UmiDebugVariablePageCount(page) != 0U) {
        UmiDebugRuntimeVariable retained; OK(UmiDebugVariablePageAt(page, 0U, &retained));
    }
    UmiDebugVariablePageDestroy(page); UmiDebugVariableTargetDestroy(target); return 0;
}
