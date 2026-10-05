/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/memory_inspection/test_platform.c
 * PURPOSE: Check live request identity, pending events, stale targets and owned result lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../variable_inspection/fixture.h"
#include "umicom/debug_runtime/memory_inspection.h"
#include "umicom/debug_runtime/scope_inspection.h"
static void Stopped(UmiDebugRuntimePlatform *platform)
{
    for (unsigned i = 0U; i < 80U; ++i)
    {
        UmiDebugRuntimePlatformSnapshot s;
        OK(umi_debug_runtime_platform_snapshot(platform, &s));
        if (s.paused)
            return;
        int handled = 0;
        UmiStatus status = umi_debug_runtime_platform_pump_event(platform, 25U, &handled);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *name = argv[2];
    UmiDebugRuntimePlatform *platform = NULL;
    OK(umi_debug_runtime_platform_create(&platform));
    Fixture f = {0};
    OpenWithService(&f, umi_debug_runtime_platform_service(platform));
    UmiDebugVariableTarget *container = NULL, *target = NULL;
    UmiDebugVariablePage *page = NULL;
    UmiDebugMemoryCapture *capture = NULL;
    if (strcmp(name, "idle") == 0 || strcmp(name, "wrong-owner") == 0)
    {
        Fixture other = {0};
        Open(&other);
        PopulateVariables(&f);
        PopulateVariables(&other);
        OK(UmiDebugVariableTargetCapture(strcmp(name, "idle") == 0 ? f.workspace : other.workspace, 0U,
                                         &target));
        CHECK(UmiDebugRuntimeInspectMemory(platform,
                                           strcmp(name, "idle") == 0 ? f.workspace : other.workspace, target,
                                           0, 256U, 500U, &capture) ==
              (strcmp(name, "idle") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_INVALID_ARGUMENT));
        CHECK(capture == NULL);
        Close(&other);
    }
    else
    {
        UmiDebugAdapterProfile profile = *umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
        (void)snprintf(profile.executable, sizeof profile.executable, "%s", argv[1]);
        (void)snprintf(profile.arguments, sizeof profile.arguments, "memory-inspection-%s", name);
        OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(f.service),
                                                     &profile));
        OK(umi_debug_runtime_platform_start(platform, profile.id, "session", "launch", "{}", 0, NULL, 2000U));
        Stopped(platform);
        OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
        UmiDebugScopeSnapshot scope;
        OK(UmiDebugScopeInspectionCapture(f.workspace, 0U, &scope, &container));
        OK(UmiDebugRuntimeInspectVariable(platform, f.workspace, container, 2000U, &page));
        OK(UmiDebugVariablePageTarget(page, 0U, &target));
        UmiDebugRuntimeVariable variable;
        OK(UmiDebugVariableTargetRead(target, &variable));
        CHECK(variable.variables_reference == 0U);
        if (strcmp(name, "stale") == 0)
        {
            UmiDebugVariableSnapshot root;
            OK(umi_debug_workspace_variable_at(f.workspace, 0U, &root));
            OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &root));
        }
        UmiDebugViewStamp before, after;
        OK(UmiDebugWorkspaceViewStamp(f.workspace, &before));
        UmiDebugRuntimePlatformSnapshot first, last;
        OK(umi_debug_runtime_platform_snapshot(platform, &first));
        UmiStatus status;
        if (strcmp(name, "bounds") == 0)
        {
            CHECK(UmiDebugRuntimeInspectMemory(platform, f.workspace, target, 0, 0U, 500U, &capture) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDebugRuntimeInspectMemory(platform, f.workspace, target, 0, 4097U, 500U, &capture) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDebugRuntimeInspectMemory(platform, f.workspace, target,
                                               UMI_DEBUG_MEMORY_OFFSET_LIMIT + 1, 256U, 500U,
                                               &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDebugRuntimeInspectMemory(platform, f.workspace, target,
                                               -UMI_DEBUG_MEMORY_OFFSET_LIMIT - 1, 256U, 500U,
                                               &capture) == UMI_STATUS_INVALID_ARGUMENT);
            status = UmiDebugRuntimeInspectMemory(platform, f.workspace, target, 0, 256U, 0U, &capture);
        }
        else
            status = UmiDebugRuntimeInspectMemory(platform, f.workspace, target,
                                                  strcmp(name, "offset") == 0 ? -16 : 0, 256U,
                                                  strcmp(name, "timeout") == 0 ? 80U : 2000U, &capture);
        OK(umi_debug_runtime_platform_snapshot(platform, &last));
        OK(UmiDebugWorkspaceViewStamp(f.workspace, &after));
        CHECK(UmiDebugViewStampEqual(&before, &after));
        UmiStatus expected = UMI_STATUS_OK;
        if (strcmp(name, "unsupported") == 0)
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        else if (strcmp(name, "no-reference") == 0)
            expected = UMI_STATUS_NOT_FOUND;
        else if (strcmp(name, "stale") == 0 || strcmp(name, "event") == 0)
            expected = UMI_STATUS_BUSY;
        else if (strcmp(name, "bounds") == 0)
            expected = UMI_STATUS_INVALID_ARGUMENT;
        else if (strcmp(name, "rejected") == 0)
            expected = UMI_STATUS_UNAVAILABLE;
        else if (strcmp(name, "malformed") == 0 || strcmp(name, "command") == 0)
            expected = UMI_STATUS_PARSE_ERROR;
        else if (strcmp(name, "timeout") == 0)
            expected = UMI_STATUS_TIMEOUT;
        else
            CHECK(strcmp(name, "success") == 0 || strcmp(name, "offset") == 0 || strcmp(name, "empty") == 0 ||
                  strcmp(name, "unreadable") == 0 || strcmp(name, "retained") == 0 ||
                  strcmp(name, "repeat") == 0);
        CHECK(status == expected && (capture != NULL) == (status == UMI_STATUS_OK));
        int sent = strcmp(name, "unsupported") != 0 && strcmp(name, "no-reference") != 0 &&
                   strcmp(name, "stale") != 0 && strcmp(name, "bounds") != 0;
        CHECK(last.adapter.messages_sent == first.adapter.messages_sent + (sent ? 1U : 0U));
        if (strcmp(name, "event") == 0)
        {
            CHECK(last.adapter.queued_events == 1U);
            CHECK(UmiDebugRuntimeCheckMemory(platform, f.workspace, target) == UMI_STATUS_BUSY);
            CHECK(UmiDebugRuntimeInspectMemory(platform, f.workspace, target, 0, 256U, 500U, &capture) ==
                  UMI_STATUS_BUSY);
            UmiDebugRuntimePlatformSnapshot again;
            OK(umi_debug_runtime_platform_snapshot(platform, &again));
            CHECK(again.adapter.messages_sent == last.adapter.messages_sent);
        }
        if (capture != NULL)
        {
            UmiDebugMemoryBytes bytes;
            OK(UmiDebugMemoryCaptureRead(capture, &bytes));
            CHECK(bytes.count == (strcmp(name, "empty") == 0 ? 0U : 6U));
            CHECK(bytes.unreadable == (strcmp(name, "unreadable") == 0 ? 20U
                                       : strcmp(name, "empty") == 0    ? 8U
                                                                       : 0U));
            OK(UmiDebugMemoryCaptureValidate(capture, f.workspace));
            if (strcmp(name, "repeat") == 0)
            {
                UmiDebugMemoryCapture *again = NULL;
                OK(UmiDebugRuntimeInspectMemory(platform, f.workspace, target, 0, 256U, 2000U, &again));
                CHECK(again != capture);
                UmiDebugMemoryCaptureDestroy(again);
            }
            size_t required = 0U;
            CHECK(UmiDebugMemoryCaptureFormat(capture, NULL, 0U, &required) == UMI_STATUS_CAPACITY_EXCEEDED &&
                  required > 0U);
            char *text = malloc(required);
            CHECK(text != NULL);
            OK(UmiDebugMemoryCaptureFormat(capture, text, required, &required));
            CHECK(strstr(text, "Address: 0x1000") != NULL);
            if (bytes.count)
                CHECK(strstr(text, "41 42 43 00 7F 80") != NULL && strstr(text, "|ABC...|") != NULL);
            if (strcmp(name, "offset") == 0)
                CHECK(strstr(text, "offset: -16") != NULL);
            free(text);
        }
        OK(umi_debug_runtime_platform_stop(platform, 1, 2000U));
        if (capture != NULL)
            CHECK(UmiDebugMemoryCaptureValidate(capture, f.workspace) != UMI_STATUS_OK);
    }
    UmiDebugVariablePageDestroy(page);
    UmiDebugVariableTargetDestroy(container);
    UmiDebugVariableTargetDestroy(target);
    Close(&f);
    umi_debug_runtime_platform_destroy(platform);
    /* Captured bytes and their report survive destruction of every live owner. */
    if (capture != NULL)
    {
        UmiDebugMemoryBytes bytes;
        OK(UmiDebugMemoryCaptureRead(capture, &bytes));
        CHECK(strcmp(bytes.address, "0x1000") == 0);
    }
    UmiDebugMemoryCaptureDestroy(capture);
    return 0;
}
