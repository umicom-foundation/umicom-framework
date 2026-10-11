/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_instructions/test_protocol.c
 * PURPOSE: Exercise stopped-frame ownership and paging through an inert native adapter process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../watch_edit/fixture.h"
#include "umicom/debug_runtime/instruction_inspection.h"
#include "umicom/debug_runtime/native_attach.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
typedef struct InstructionStackRequest
{
    UmiDebugRuntimePlatform *platform;
    UmiStatus status;
} InstructionStackRequest;
static DWORD WINAPI InspectOnSmallStack(LPVOID data)
{
    InstructionStackRequest *request = data;
    request->status = UmiDebugRuntimePlatformInspectStopped(request->platform, 2000U);
    return 0;
}
#endif
/* The joined worker temporarily owns all debugger access. No GUI or main-thread
 * debugger action runs until it finishes. Windows gets an explicit 1 MiB reserve
 * so a larger executable-wide stack setting cannot hide an oversized local. */
static void InspectStack(UmiDebugRuntimePlatform *platform, int small_stack)
{
#ifdef _WIN32
    if (small_stack)
    {
        InstructionStackRequest request = {platform, UMI_STATUS_INVALID_STATE};
        HANDLE worker = CreateThread(NULL, 1024U * 1024U, InspectOnSmallStack, &request,
                                     STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
        CHECK(worker != NULL);
        CHECK(WaitForSingleObject(worker, 10000U) == WAIT_OBJECT_0);
        CHECK(CloseHandle(worker));
        OK(request.status);
        return;
    }
#else
    (void)small_stack;
#endif
    OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
}

static void ObserveStop(UmiDebugRuntimePlatform *platform)
{
    for (unsigned i = 0U; i < 80U; ++i)
    {
        UmiDebugRuntimePlatformSnapshot state;
        OK(umi_debug_runtime_platform_snapshot(platform, &state));
        if (state.paused)
            return;
        int handled = 0;
        UmiStatus status = umi_debug_runtime_platform_pump_event(platform, 25U, &handled);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_TIMEOUT ||
              status == UMI_STATUS_NOT_FOUND);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    CHECK(argc == 4);
    const char *name = argv[1];
    char mode[40], settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY];
    const char *known[] = {"normal",        "unsupported",     "missing-reference",
                           "changed-frame", "capture-refuse",  "capture-timeout",
                           "capture-event", "refuse",          "timeout",
                           "malformed",     "continued",       "short",
                           "stale",         "wrong-workspace", "wrong-service",
                           "closed",        "reconnect",       "invalid-options",
                           "paging",        "small-stack",     "stack-invalid"};
    bool found = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
        if (strcmp(name, known[i]) == 0)
            found = true;
    CHECK(found);
    snprintf(mode, sizeof mode, "instructions-%s", name);
    OK(umi_path_join(argv[3], "attach-fixture-mode.txt", settings, sizeof settings));
    OK(umi_path_join(argv[3], "attach-fixture-requests.jsonl", transcript, sizeof transcript));
    OK(umi_fs_write_text(settings, mode));
    OK(umi_fs_write_text(transcript, ""));
    UmiDebugRuntimePlatform *platform = NULL;
    OK(umi_debug_runtime_platform_create(&platform));
    Fixture fixture = {0};
    OpenWithService(&fixture, umi_debug_runtime_platform_service(platform));
    UmiDebugNativeAttachOptions options = {"gdb", argv[2], "", argv[3], "", 123456U};
    OK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U));
    ObserveStop(platform);
    InspectStack(platform, strcmp(name, "small-stack") == 0);
    OK(umi_debug_workspace_refresh(fixture.workspace));
    UmiDebugInstructionTarget *target = NULL;
    UmiStatus expected = strcmp(name, "unsupported") == 0         ? UMI_STATUS_NOT_IMPLEMENTED
                         : strcmp(name, "missing-reference") == 0 ? UMI_STATUS_NOT_FOUND
                         : strcmp(name, "changed-frame") == 0     ? UMI_STATUS_INVALID_STATE
                         : strcmp(name, "capture-refuse") == 0    ? UMI_STATUS_UNAVAILABLE
                         : strcmp(name, "capture-timeout") == 0   ? UMI_STATUS_TIMEOUT
                         : strcmp(name, "capture-event") == 0     ? UMI_STATUS_BUSY
                                                                  : UMI_STATUS_OK;
    CHECK(UmiDebugInstructionTargetCapture(platform, fixture.workspace,
                                           strcmp(name, "capture-timeout") == 0 ? 100U : 2000U,
                                           &target) == expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(target == NULL);
        Close(&fixture);
        umi_debug_runtime_platform_destroy(platform);
        return 0;
    }
    UmiDebugInstructionContext context;
    OK(UmiDebugInstructionTargetRead(target, &context));
    CHECK(context.frame_id == 0U && context.thread_id == 1U);
    CHECK(strcmp(context.reference, "0x1000") == 0 && context.connection.generation != 0U);
    UmiDebugInstructionPage *page = malloc(sizeof *page), *before = malloc(sizeof *before);
    CHECK(page != NULL && before != NULL);
    memset(page, 0xA5, sizeof *page);
    memcpy(before, page, sizeof *page);
    Fixture other = {0};
    UmiDebugWorkspace *workspace = fixture.workspace;
    bool another = false;
    if (strcmp(name, "wrong-workspace") == 0 || strcmp(name, "wrong-service") == 0)
    {
        OpenWithService(&other, strcmp(name, "wrong-service") == 0 ? NULL : fixture.service);
        workspace = other.workspace;
        another = true;
        expected =
            strcmp(name, "wrong-service") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_BUSY;
    }
    if (strcmp(name, "stale") == 0)
    {
        OK(umi_debug_runtime_platform_refresh_stack(platform, 1U, 2000U));
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "closed") == 0)
    {
        OK(umi_debug_runtime_platform_stop(platform, 0, 2000U));
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "reconnect") == 0)
    {
        OK(umi_debug_runtime_platform_stop(platform, 0, 2000U));
        OK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U));
        ObserveStop(platform);
        OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "refuse") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(name, "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    if (strcmp(name, "malformed") == 0 || strcmp(name, "short") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(name, "continued") == 0)
        expected = UMI_STATUS_BUSY;
    if (strcmp(name, "invalid-options") == 0)
    {
        CHECK(UmiDebugInstructionPageRead(platform, workspace, target, 0, 0, 0U, 1, 100U, page) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDebugInstructionPageRead(platform, workspace, target, INT64_MAX, 0, 2U, 1, 100U,
                                          page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDebugInstructionPageRead(platform, workspace, target, 0, INT64_MIN, 2U, 1, 100U,
                                          page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDebugInstructionPageRead(platform, workspace, target, 0, 0, 2U, 2, 100U, page) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(page, before, sizeof *page) == 0);
    }
    if (strcmp(name, "stack-invalid") == 0)
    {
        UmiDebugViewStamp before_refresh, after_refresh;
        OK(UmiDebugWorkspaceViewStamp(fixture.workspace, &before_refresh));
        CHECK(umi_debug_runtime_platform_refresh_stack(platform, 1U, 2000U) ==
              UMI_STATUS_PARSE_ERROR);
        OK(UmiDebugWorkspaceViewStamp(fixture.workspace, &after_refresh));
        CHECK(UmiDebugViewStampEqual(&before_refresh, &after_refresh));
    }
    UmiDebugViewStamp start, finish;
    OK(UmiDebugWorkspaceViewStamp(fixture.workspace, &start));
    UmiStatus status =
        UmiDebugInstructionPageRead(platform, workspace, target, 0, 0, 2U, 1,
                                    strcmp(name, "timeout") == 0 ? 100U : 2000U, page);
    CHECK(status == expected);
    OK(UmiDebugWorkspaceViewStamp(fixture.workspace, &finish));
    CHECK(UmiDebugViewStampEqual(&start, &finish));
    if (status != UMI_STATUS_OK)
        CHECK(memcmp(page, before, sizeof *page) == 0);
    else
    {
        CHECK(page->count == 2U && strcmp(page->items[1].presentation_hint, "invalid") == 0);
        CHECK(page->items[1].has_source &&
              strcmp(page->items[1].source.path, "/untrusted/fixture.c") == 0);
    }
    if (strcmp(name, "paging") == 0)
    {
        OK(UmiDebugInstructionPageRead(platform, workspace, target, -4, -2, 2U, 0, 2000U, page));
        OK(UmiDebugInstructionPageRead(platform, workspace, target, 0, 2, 64U, 1, 2000U, page));
        CHECK(page->count == 64U);
    }
    char *log = NULL;
    size_t bytes = 0U;
    OK(umi_fs_read_text(transcript, &log, &bytes));
    CHECK(strstr(log, "\"command\":\"evaluate\"") == NULL &&
          strstr(log, "\"command\":\"continue\"") == NULL &&
          strstr(log, "\"command\":\"writeMemory\"") == NULL);
    if (strcmp(name, "paging") == 0)
        CHECK(strstr(log, "\"offset\":-4") != NULL &&
              strstr(log, "\"instructionOffset\":-2") != NULL &&
              strstr(log, "\"resolveSymbols\":false") != NULL);
    if (another)
        Close(&other);
    Close(&fixture);
    umi_debug_runtime_platform_destroy(platform);
    UmiDebugInstructionContext saved;
    OK(UmiDebugInstructionTargetRead(target, &saved));
    CHECK(UmiDebugInstructionContextEqual(&context, &saved));
    UmiDebugInstructionTargetDestroy(target);
    free(page);
    free(before);
    umi_fs_free_text(log);
    return 0;
}
