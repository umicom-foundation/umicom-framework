/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_inspection/test_platform.c
 * PURPOSE: Exercise actual child requests with interleaved events, rejected replies and stale references.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
typedef struct RootThreadRequest { UmiDebugRuntimePlatform *platform; char scope[128]; UmiStatus status; } RootThreadRequest;
static DWORD WINAPI ReadRootsOnSmallStack(LPVOID data)
{
    RootThreadRequest *request = data;
    request->status = umi_debug_runtime_platform_refresh_variables(request->platform, request->scope, 1U, 2000U);
    return 0;
}
#endif
static void WaitStopped(UmiDebugRuntimePlatform *platform)
{
    for (unsigned i = 0U; i < 80U; ++i) {
        UmiDebugRuntimePlatformSnapshot state; OK(umi_debug_runtime_platform_snapshot(platform, &state));
        if (state.paused) return;
        int handled = 0; UmiStatus status = umi_debug_runtime_platform_pump_event(platform, 25U, &handled);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    CHECK(argc == 3); const char *name = argv[2]; UmiDebugRuntimePlatform *platform = NULL;
    OK(umi_debug_runtime_platform_create(&platform)); Fixture f = {0};
    OpenWithService(&f, umi_debug_runtime_platform_service(platform));
    UmiDebugVariableTarget *target = NULL; UmiDebugVariablePage *page = NULL;
    if (strcmp(name, "idle") == 0 || strcmp(name, "wrong-owner") == 0) {
        PopulateVariables(&f); OK(UmiDebugVariableTargetCapture(f.workspace, 0, &target));
        if (strcmp(name, "idle") == 0) CHECK(UmiDebugRuntimeInspectVariable(platform, f.workspace, target, 500U, &page) == UMI_STATUS_INVALID_STATE && page == NULL);
        else {
            Fixture other = {0}; Open(&other); PopulateVariables(&other); UmiDebugVariableTarget *foreign = NULL;
            OK(UmiDebugVariableTargetCapture(other.workspace, 0, &foreign));
            CHECK(UmiDebugRuntimeInspectVariable(platform, other.workspace, foreign, 500U, &page) == UMI_STATUS_INVALID_ARGUMENT && page == NULL);
            UmiDebugVariableTargetDestroy(foreign); Close(&other);
        }
    } else {
        UmiDebugAdapterProfile profile = *umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
        (void)snprintf(profile.executable, sizeof profile.executable, "%s", argv[1]);
        (void)snprintf(profile.arguments, sizeof profile.arguments, "variable-inspection-%s", name);
        OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(f.service), &profile));
        OK(umi_debug_runtime_platform_start(platform, profile.id, "session", "launch", "{}", 0, NULL, 2000U));
        WaitStopped(platform); OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
        if (strcmp(name, "small-stack") == 0) {
            UmiDebugScopeSnapshot scope; OK(umi_debug_workspace_refresh(f.workspace));
            OK(umi_debug_workspace_scope_at(f.workspace, 0U, &scope));
#ifdef _WIN32
            /* No main-thread access occurs while the joined worker temporarily
             * owns this request. Reserve 1 MiB rather than inheriting a link flag. */
            RootThreadRequest request = {0}; request.platform = platform; strcpy(request.scope, scope.id);
            request.status = UMI_STATUS_INVALID_STATE;
            HANDLE thread = CreateThread(NULL, 1024U * 1024U, ReadRootsOnSmallStack, &request,
                STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
            CHECK(thread != NULL); CHECK(WaitForSingleObject(thread, 10000U) == WAIT_OBJECT_0);
            CHECK(CloseHandle(thread)); CHECK(request.status == UMI_STATUS_OK);
#else
            OK(umi_debug_runtime_platform_refresh_variables(platform, scope.id, 1U, 2000U));
#endif
        }
        UmiDebugVariableSnapshot root; OK(umi_debug_workspace_variable_at(f.workspace, 0U, &root));
        if (strcmp(name, "scalar") == 0) {
            root.variables_reference = 0U; OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &root));
        }
        OK(UmiDebugVariableTargetCapture(f.workspace, 0U, &target));
        if (strcmp(name, "stale") == 0) OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &root));
        UmiDebugViewStamp before, after; OK(UmiDebugWorkspaceViewStamp(f.workspace, &before));
        UmiDebugRuntimePlatformSnapshot start, finish; OK(umi_debug_runtime_platform_snapshot(platform, &start));
        UmiStatus status = UmiDebugRuntimeInspectVariable(platform, f.workspace, target, strcmp(name, "timeout") == 0 ? 80U : 2000U, &page);
        OK(umi_debug_runtime_platform_snapshot(platform, &finish)); OK(UmiDebugWorkspaceViewStamp(f.workspace, &after));
        CHECK(UmiDebugViewStampEqual(&before, &after)); /* Child reads never replace root registries. */
        if (strcmp(name, "success") == 0 || strcmp(name, "small-stack") == 0 || strcmp(name, "nested") == 0 || strcmp(name, "repeat") == 0 ||
            strcmp(name, "empty") == 0 || strcmp(name, "unicode") == 0 || strcmp(name, "cycle") == 0) {
            CHECK(status == UMI_STATUS_OK && page != NULL);
            CHECK(finish.adapter.messages_sent == start.adapter.messages_sent + 1U);
            if (strcmp(name, "empty") == 0) CHECK(UmiDebugVariablePageCount(page) == 0U);
            else {
                UmiDebugRuntimeVariable value; OK(UmiDebugVariablePageAt(page, 0U, &value));
                if (strcmp(name, "unicode") == 0) CHECK(strcmp(value.name, "caf\xc3\xa9") == 0 && strcmp(value.value, "\xf0\x9f\x9a\x80") == 0);
                else if (strcmp(name, "cycle") == 0 || strcmp(name, "nested") == 0) {
                    UmiDebugVariableTarget *child = NULL; UmiDebugVariablePage *nested = NULL;
                    OK(UmiDebugVariablePageTarget(page, 0U, &child));
                    UmiStatus result = UmiDebugRuntimeInspectVariable(platform, f.workspace, child, 2000U, &nested);
                    if (strcmp(name, "cycle") == 0) CHECK(result == UMI_STATUS_ALREADY_EXISTS && nested == NULL);
                    else { CHECK(result == UMI_STATUS_OK); OK(UmiDebugVariablePageAt(nested, 0U, &value)); CHECK(strcmp(value.value, "7") == 0); }
                    UmiDebugVariablePageDestroy(nested); UmiDebugVariableTargetDestroy(child);
                } else CHECK(UmiDebugVariablePageCount(page) == 2U && strcmp(value.name, "item") == 0);
                if (strcmp(name, "repeat") == 0) {
                    UmiDebugVariablePage *again = NULL; OK(UmiDebugRuntimeInspectVariable(platform, f.workspace, target, 2000U, &again));
                    CHECK(again != page && UmiDebugVariablePageCount(again) == 2U); UmiDebugVariablePageDestroy(again);
                }
            }
        } else {
            UmiStatus expected = UMI_STATUS_PARSE_ERROR;
            if (strcmp(name, "stale") == 0 || strcmp(name, "event") == 0) expected = UMI_STATUS_BUSY;
            else if (strcmp(name, "scalar") == 0) expected = UMI_STATUS_INVALID_STATE;
            else if (strcmp(name, "rejected") == 0) expected = UMI_STATUS_UNAVAILABLE;
            else if (strcmp(name, "timeout") == 0) expected = UMI_STATUS_TIMEOUT;
            else CHECK(strcmp(name, "malformed") == 0 || strcmp(name, "command") == 0);
            CHECK(status == expected && page == NULL);
            if (strcmp(name, "stale") == 0 || strcmp(name, "scalar") == 0) CHECK(finish.adapter.messages_sent == start.adapter.messages_sent);
            if (strcmp(name, "event") == 0) {
                CHECK(finish.adapter.queued_events == 1U);
                CHECK(UmiDebugRuntimeInspectVariable(platform, f.workspace, target, 2000U, &page) == UMI_STATUS_BUSY && page == NULL);
                UmiDebugRuntimePlatformSnapshot blocked; OK(umi_debug_runtime_platform_snapshot(platform, &blocked));
                CHECK(blocked.adapter.messages_sent == finish.adapter.messages_sent);
                int handled = 0; OK(umi_debug_runtime_platform_pump_event(platform, 0, &handled)); CHECK(handled);
                OK(umi_debug_runtime_platform_snapshot(platform, &blocked)); CHECK(!blocked.paused);
            }
        }
        OK(umi_debug_runtime_platform_stop(platform, 1, 2000U));
        /* Captured text remains readable after the native session closes. */
        if (page != NULL && UmiDebugVariablePageCount(page) != 0U) {
            UmiDebugRuntimeVariable value; OK(UmiDebugVariablePageAt(page, 0U, &value));
        }
    }
    UmiDebugVariablePageDestroy(page); UmiDebugVariableTargetDestroy(target);
    Close(&f); umi_debug_runtime_platform_destroy(platform); return 0;
}
