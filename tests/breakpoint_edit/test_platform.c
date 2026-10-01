/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/breakpoint_edit/test_platform.c
 * PURPOSE: Exercise real process transport and complete source-set confirmation against a controlled peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/platform.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int failed = 0; UmiDebugRuntimePlatform *platform = NULL;
    OK(umi_debug_runtime_platform_create(&platform));
    UmiDebugService *service = umi_debug_runtime_platform_service(platform);
    UmiDebugAdapterProfile profile = *umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
    (void)snprintf(profile.executable, sizeof profile.executable, "%s", argv[1]);
    (void)snprintf(profile.arguments, sizeof profile.arguments, "properties-%s", argv[2]);
    OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(service), &profile));
    /* Start only the deterministic protocol peer, without any initial breakpoint.
     * It does not debug a process and cannot qualify a real GDB/LLDB session. */
    OK(umi_debug_runtime_platform_start(platform, profile.id, "session", "launch", "{}", 0, NULL, 2000U));
    UmiDebugBreakpointRegistry *registry = umi_debug_service_breakpoint(service);
    UmiDebugBreakpointSnapshot item = {0}; strcpy(item.id, "first"); strcpy(item.uri, "main.c");
    item.line = 13; item.column = 1; item.enabled = 1; item.verified = 1;
    strcpy(item.condition, "count > 3"); strcpy(item.log_message, "count={count}");
    OK(umi_debug_breakpoint_registry_upsert(registry, &item));
    UmiStatus status = umi_debug_runtime_platform_sync_breakpoints(platform, "main.c", 2000U);
    UmiDebugBreakpointSnapshot actual; OK(umi_debug_breakpoint_registry_find(registry, item.id, &actual));
    CHECK(strcmp(actual.condition, item.condition) == 0 && strcmp(actual.log_message, item.log_message) == 0);
    if (strcmp(argv[2], "success") == 0) CHECK(status == UMI_STATUS_OK && actual.verified && actual.line == 15);
    else if (strcmp(argv[2], "unsupported") == 0) CHECK(status == UMI_STATUS_UNAVAILABLE && actual.verified && actual.line == 13);
    else {
        CHECK(strcmp(argv[2], "short") == 0 || strcmp(argv[2], "malformed") == 0 || strcmp(argv[2], "rejected") == 0);
        CHECK(status == (strcmp(argv[2], "rejected") == 0 ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_PARSE_ERROR));
        CHECK(!actual.verified && actual.line == 13);
    }
    OK(umi_debug_runtime_platform_stop(platform, 1, 2000U));
done:
    umi_debug_runtime_platform_destroy(platform); return failed;
}
