/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_restart/test_session.c
 * PURPOSE: Exercise restart through production transport and retained debugger models.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            result = 1;                                                                                      \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiStatus WaitStopped(UmiDebugRuntimePlatform *platform)
{
    for (unsigned attempt = 0U; attempt < 100U; ++attempt)
    {
        UmiDebugRuntimePlatformSnapshot state;
        UmiStatus status = umi_debug_runtime_platform_snapshot(platform, &state);
        if (status != UMI_STATUS_OK || state.paused)
            return status;
        int handled = 0;
        status = umi_debug_runtime_platform_pump_event(platform, 25U, &handled);
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND)
            return status;
    }
    return UMI_STATUS_TIMEOUT;
}
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    int result = 0;
    UmiDebugRuntimePlatform *platform = NULL;
    OK(umi_debug_runtime_platform_create(&platform));
    CHECK(UmiDebugRuntimePlatformCheckRestart(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugRuntimePlatformCheckRestart(platform) == UMI_STATUS_INVALID_STATE);
    UmiDebugService *service = umi_debug_runtime_platform_service(platform);
    UmiDebugAdapterProfile profile = *umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
    CHECK(strlen(argv[1]) < sizeof profile.executable);
    strcpy(profile.executable, argv[1]);
    int length = snprintf(profile.arguments, sizeof profile.arguments, "session-restart-%s", argv[2]);
    CHECK(length > 0 && (size_t)length < sizeof profile.arguments);
    OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(service), &profile));
    if (strncmp(argv[2], "reconfigure", strlen("reconfigure")) == 0)
    {
        UmiDebugBreakpointSnapshot breakpoint = {0};
        strcpy(breakpoint.id, "restart-breakpoint");
        strcpy(breakpoint.uri, "notes.c");
        breakpoint.line = 13U;
        breakpoint.enabled = 1;
        OK(umi_debug_breakpoint_registry_upsert(umi_debug_service_breakpoint(service), &breakpoint));
    }
    OK(umi_debug_runtime_platform_start(platform, profile.id, "restart-session", "launch", "{}", 0, NULL,
                                        2000U));
    OK(WaitStopped(platform));
    OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
    UmiDebugWatchSnapshot watch = {0};
    strcpy(watch.id, "saved-watch");
    strcpy(watch.expression, "savedNotes");
    strcpy(watch.value, "2");
    strcpy(watch.type, "int");
    strcpy(watch.session_id, "restart-session");
    watch.enabled = 1;
    watch.valid = 1;
    OK(umi_debug_watch_registry_upsert(umi_debug_service_watch(service), &watch));
    UmiDebugRuntimePlatformSnapshot before, after;
    OK(umi_debug_runtime_platform_snapshot(platform, &before));
    const uint64_t variables = umi_debug_variable_registry_revision(umi_debug_service_variable(service));
    UmiStatus status = umi_debug_runtime_platform_restart(platform, strcmp(argv[2], "zero") == 0 ? 0U : 250U);
    OK(umi_debug_runtime_platform_snapshot(platform, &after));
    if (strcmp(argv[2], "unsupported") == 0 || strcmp(argv[2], "zero") == 0 || strcmp(argv[2], "queued") == 0)
    {
        UmiStatus expected = strcmp(argv[2], "unsupported") == 0 ? UMI_STATUS_NOT_IMPLEMENTED
                             : strcmp(argv[2], "zero") == 0      ? UMI_STATUS_INVALID_ARGUMENT
                                                                 : UMI_STATUS_BUSY;
        CHECK(status == expected && after.paused && after.revision == before.revision);
        CHECK(umi_debug_variable_registry_revision(umi_debug_service_variable(service)) == variables);
        OK(umi_debug_watch_registry_find(umi_debug_service_watch(service), "saved-watch", &watch));
        CHECK(watch.valid && strcmp(watch.value, "2") == 0);
    }
    else
    {
        CHECK(!after.paused && after.revision > before.revision);
        CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(service)) == 0U);
        CHECK(umi_debug_thread_registry_count(umi_debug_service_thread(service)) == 0U);
        CHECK(umi_debug_stack_frame_registry_count(umi_debug_service_stack_frame(service)) == 0U);
        CHECK(umi_debug_scope_registry_count(umi_debug_service_scope(service)) == 0U);
        OK(umi_debug_watch_registry_find(umi_debug_service_watch(service), "saved-watch", &watch));
        CHECK(!watch.valid && watch.value[0] == '\0' && watch.session_id[0] == '\0');
        CHECK(watch.enabled && strcmp(watch.expression, "savedNotes") == 0);
        if (strcmp(argv[2], "rejected") == 0 || strcmp(argv[2], "timeout") == 0 ||
            strcmp(argv[2], "wrong-command") == 0 || strcmp(argv[2], "reconfigure-rejected") == 0 ||
            strcmp(argv[2], "reconfigure-timeout") == 0)
        {
            UmiStatus expected =
                (strcmp(argv[2], "rejected") == 0 || strcmp(argv[2], "reconfigure-rejected") == 0)
                    ? UMI_STATUS_UNAVAILABLE
                : (strcmp(argv[2], "timeout") == 0 || strcmp(argv[2], "reconfigure-timeout") == 0)
                    ? UMI_STATUS_TIMEOUT
                    : UMI_STATUS_PARSE_ERROR;
            CHECK(status == expected);
            CHECK(umi_debug_runtime_platform_restart(platform, 250U) == UMI_STATUS_INVALID_STATE);
        }
        else
        {
            CHECK(status == UMI_STATUS_OK);
            OK(WaitStopped(platform));
            OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
            CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(service)) == 1U);
        }
    }
    OK(umi_debug_runtime_platform_stop(platform, 1, 2000U));
    CHECK(UmiDebugRuntimePlatformCheckRestart(platform) == UMI_STATUS_INVALID_STATE);
done:
    umi_debug_runtime_platform_destroy(platform);
    return result;
}
