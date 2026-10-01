/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/watch_edit/test_platform.c
 * PURPOSE: Verify guarded watch execution through the real transport and a deterministic peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/debug_runtime/watch_evaluation.h"
static UmiStatus WaitStopped(UmiDebugRuntimePlatform *platform)
{
    for (unsigned i = 0U; i < 40U; ++i) {
        UmiDebugRuntimePlatformSnapshot value;
        UmiStatus status = umi_debug_runtime_platform_snapshot(platform, &value);
        if (status != UMI_STATUS_OK || value.paused) return status;
        int handled = 0; status = umi_debug_runtime_platform_pump_event(platform, 25U, &handled);
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    }
    return UMI_STATUS_TIMEOUT;
}
int main(int argc, char **argv)
{
    CHECK(argc == 3); const char *name = argv[2];
    UmiDebugRuntimePlatform *platform = NULL; OK(umi_debug_runtime_platform_create(&platform));
    Fixture f = {0}; OpenWithService(&f, umi_debug_runtime_platform_service(platform));
    UmiDebugWatchEdit *edit = NULL;
    if (strcmp(name, "idle") == 0 || strcmp(name, "wrong-owner") == 0) {
        OK(UmiDebugWatchEditCapture(f.workspace, 0, &edit));
        if (strcmp(name, "idle") == 0) CHECK(UmiDebugRuntimeEvaluateWatchEdit(platform, f.workspace, edit, 500U) == UMI_STATUS_INVALID_STATE);
        else {
            Fixture other = {0}; Open(&other); UmiDebugWatchEdit *foreign = NULL;
            OK(UmiDebugWatchEditCapture(other.workspace, 0, &foreign));
            CHECK(UmiDebugRuntimeEvaluateWatchEdit(platform, other.workspace, foreign, 500U) == UMI_STATUS_INVALID_ARGUMENT);
            UmiDebugWatchEditDestroy(foreign); Close(&other);
        }
    } else {
        UmiDebugAdapterProfile profile = *umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
        (void)snprintf(profile.executable, sizeof profile.executable, "%s", argv[1]);
        (void)snprintf(profile.arguments, sizeof profile.arguments, "watch-edit-%s", name);
        OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(f.service), &profile));
        OK(umi_debug_runtime_platform_start(platform, profile.id, "session", "launch", "{}", 0, NULL, 2000U));
        OK(WaitStopped(platform));
        if (strcmp(name, "uninspected") != 0) OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
        if (strcmp(name, "disabled") == 0) { UmiDebugWatchSnapshot value = Read(&f); value.enabled = 0; OK(umi_debug_watch_registry_upsert(f.registry, &value)); }
        OK(UmiDebugWatchEditCapture(f.workspace, 0U, &edit));
        if (strcmp(name, "stale") == 0) { UmiDebugWatchSnapshot value = Read(&f); strcpy(value.expression, "changed"); OK(umi_debug_watch_registry_upsert(f.registry, &value)); }
        UmiDebugWatchSnapshot before = Read(&f);
        UmiDebugRuntimePlatformSnapshot start, finish; OK(umi_debug_runtime_platform_snapshot(platform, &start));
        UmiStatus status = UmiDebugRuntimeEvaluateWatchEdit(platform, f.workspace, edit, 2000U);
        OK(umi_debug_runtime_platform_snapshot(platform, &finish));
        UmiDebugWatchSnapshot after = Read(&f);
        if (strcmp(name, "success") == 0 || strcmp(name, "empty") == 0 || strcmp(name, "unicode") == 0) {
            CHECK(status == UMI_STATUS_OK && after.valid && after.enabled && after.revision > before.revision);
            CHECK(strcmp(after.session_id, "session") == 0 && strcmp(after.expression, before.expression) == 0);
            CHECK(strcmp(after.value, strcmp(name, "empty") == 0 ? "" : strcmp(name, "unicode") == 0 ? "caf\xc3\xa9 \xf0\x9f\x9a\x80" : "2") == 0);
            CHECK(finish.adapter.messages_sent == start.adapter.messages_sent + 1U);
        } else {
            UmiStatus expected = UMI_STATUS_PARSE_ERROR;
            if (strcmp(name, "disabled") == 0 || strcmp(name, "uninspected") == 0) expected = UMI_STATUS_INVALID_STATE;
            else if (strcmp(name, "stale") == 0 || strcmp(name, "event") == 0) expected = UMI_STATUS_BUSY;
            else if (strcmp(name, "rejected") == 0) expected = UMI_STATUS_UNAVAILABLE;
            else if (strcmp(name, "long") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
            else CHECK(strcmp(name, "malformed") == 0 || strcmp(name, "command") == 0);
            CHECK(status == expected && after.revision == before.revision && strcmp(after.value, before.value) == 0);
            if (strcmp(name, "disabled") == 0 || strcmp(name, "uninspected") == 0 || strcmp(name, "stale") == 0)
                CHECK(finish.adapter.messages_sent == start.adapter.messages_sent);
            if (strcmp(name, "event") == 0) {
                CHECK(finish.adapter.queued_events == 1U);
                CHECK(UmiDebugRuntimeEvaluateWatchEdit(platform, f.workspace, edit, 2000U) == UMI_STATUS_BUSY);
                UmiDebugRuntimePlatformSnapshot blocked; OK(umi_debug_runtime_platform_snapshot(platform, &blocked));
                CHECK(blocked.adapter.messages_sent == finish.adapter.messages_sent);
                int handled = 0; OK(umi_debug_runtime_platform_pump_event(platform, 0U, &handled));
                CHECK(handled); OK(umi_debug_runtime_platform_snapshot(platform, &blocked)); CHECK(!blocked.paused);
            }
        }
        OK(umi_debug_runtime_platform_stop(platform, 1, 2000U));
    }
    UmiDebugWatchEditDestroy(edit); Close(&f); umi_debug_runtime_platform_destroy(platform); return 0;
}
