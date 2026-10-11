/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_instructions/test_control.c
 * PURPOSE: Verify debugger control requests retire old captures before uncertain replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../watch_edit/fixture.h"
#include "umicom/debug_runtime/instruction_inspection.h"
#include "umicom/debug_runtime/native_attach.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <limits.h>

typedef UmiStatus (*ControlOperation)(UmiDebugRuntimePlatform *, uint64_t, uint32_t);

/* Poll the real framed fixture transport until its stop has been published.
 * No sleeping target or real process handle is needed to exercise ownership. */
static void ControlObserveStop(UmiDebugRuntimePlatform *platform)
{
    for (unsigned index = 0U; index < 80U; ++index)
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
static size_t CommandCount(const char *log, const char *command)
{
    char needle[80];
    snprintf(needle, sizeof needle, "\"command\":\"%s\"", command);
    size_t count = 0U;
    for (const char *cursor = log; (cursor = strstr(cursor, needle)) != NULL;
         cursor += strlen(needle))
        ++count;
    return count;
}
int main(int argc, char **argv)
{
    CHECK(argc == 4);
    const char *name = argv[1];
    const char *known[] = {"continue",      "next",     "into",      "out",      "back",
                           "reverse",       "pause",    "pause-ack", "timeout",  "refuse",
                           "wrong-command", "bad-body", "partial",   "bodyless", "early-stop",
                           "late-stop",     "invalid",  "duplicate"};
    bool known_case = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
        if (strcmp(name, known[i]) == 0)
            known_case = true;
    CHECK(known_case);
    char mode[32], settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY];
    snprintf(mode, sizeof mode, "control-%s", name);
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
    ControlObserveStop(platform);
    OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
    OK(umi_debug_workspace_refresh(fixture.workspace));
    /* Seed a visible watch value at this stop. Retirement must clear its
     * validity while retaining the user's expression for later evaluation. */
    UmiDebugWatchSnapshot visible_watch = Read(&fixture);
    visible_watch.valid = 1;
    strcpy(visible_watch.value, "captured value");
    OK(umi_debug_watch_registry_upsert(fixture.registry, &visible_watch));
    UmiDebugInstructionTarget *target = NULL;
    OK(UmiDebugInstructionTargetCapture(platform, fixture.workspace, 2000U, &target));
    UmiDebugInstructionPage *page = malloc(sizeof *page), *unchanged = malloc(sizeof *unchanged);
    CHECK(page != NULL && unchanged != NULL);
    memset(page, 0xA5, sizeof *page);
    memcpy(unchanged, page, sizeof *page);
    UmiDebugViewStamp original, after;
    OK(UmiDebugWorkspaceViewStamp(fixture.workspace, &original));

    ControlOperation operation = umi_debug_runtime_platform_continue;
    const char *command = "continue";
    if (strcmp(name, "next") == 0 || strcmp(name, "bodyless") == 0)
    {
        operation = umi_debug_runtime_platform_step_over;
        command = "next";
    }
    else if (strcmp(name, "into") == 0)
    {
        operation = umi_debug_runtime_platform_step_into;
        command = "stepIn";
    }
    else if (strcmp(name, "out") == 0)
    {
        operation = umi_debug_runtime_platform_step_out;
        command = "stepOut";
    }
    else if (strcmp(name, "back") == 0)
    {
        operation = umi_debug_runtime_platform_step_back;
        command = "stepBack";
    }
    else if (strcmp(name, "reverse") == 0)
    {
        operation = umi_debug_runtime_platform_reverse_continue;
        command = "reverseContinue";
    }

    if (strcmp(name, "invalid") == 0)
    {
        /* Refused arguments neither send a command nor retire the current stop. */
        CHECK(operation(platform, 0U, 100U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(operation(platform, (uint64_t)INT32_MAX + 1U, 100U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(operation(platform, 1U, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_debug_runtime_platform_pause(platform, 1U, 100U) == UMI_STATUS_INVALID_STATE);
        OK(UmiDebugWorkspaceViewStamp(fixture.workspace, &after));
        CHECK(UmiDebugViewStampEqual(&original, &after));
        OK(UmiDebugInstructionPageRead(platform, fixture.workspace, target, 0, 0, 2U, 0, 2000U,
                                       page));
    }
    else
    {
        UmiStatus expected = strcmp(name, "timeout") == 0  ? UMI_STATUS_TIMEOUT
                             : strcmp(name, "refuse") == 0 ? UMI_STATUS_UNAVAILABLE
                             : strcmp(name, "wrong-command") == 0 || strcmp(name, "bad-body") == 0
                                 ? UMI_STATUS_PARSE_ERROR
                                 : UMI_STATUS_OK;
        CHECK(operation(platform, 1U, strcmp(name, "timeout") == 0 ? 100U : 2000U) == expected);
        UmiDebugRuntimePlatformSnapshot state;
        OK(umi_debug_runtime_platform_snapshot(platform, &state));
        CHECK(!state.paused);
        CHECK(umi_debug_thread_registry_count(umi_debug_service_thread(fixture.service)) == 0U);
        CHECK(umi_debug_stack_frame_registry_count(
                  umi_debug_service_stack_frame(fixture.service)) == 0U);
        CHECK(umi_debug_scope_registry_count(umi_debug_service_scope(fixture.service)) == 0U);
        CHECK(umi_debug_variable_registry_count(umi_debug_service_variable(fixture.service)) == 0U);
        UmiDebugWatchSnapshot watch = Read(&fixture);
        CHECK(!watch.valid);
        OK(UmiDebugWorkspaceViewStamp(fixture.workspace, &after));
        CHECK(!UmiDebugViewStampEqual(&original, &after));
        CHECK(UmiDebugInstructionPageRead(platform, fixture.workspace, target, 0, 0, 2U, 0, 100U,
                                          page) != UMI_STATUS_OK);
        CHECK(memcmp(page, unchanged, sizeof *page) == 0);
        if (strcmp(name, "duplicate") == 0 || expected != UMI_STATUS_OK)
        {
            UmiStatus again = operation(platform, 1U, 100U);
            CHECK(again == UMI_STATUS_INVALID_STATE || again == UMI_STATUS_BUSY);
        }
        if (strcmp(name, "pause") == 0 || strcmp(name, "pause-ack") == 0)
        {
            OK(umi_debug_runtime_platform_pause(platform, 1U, 2000U));
            OK(umi_debug_runtime_platform_snapshot(platform, &state));
            CHECK(!state.paused);
        }
        if (strcmp(name, "early-stop") == 0 || strcmp(name, "late-stop") == 0 ||
            strcmp(name, "pause") == 0)
        {
            ControlObserveStop(platform);
            OK(UmiDebugRuntimePlatformInspectStopped(platform, 2000U));
            OK(umi_debug_workspace_refresh(fixture.workspace));
            /* A new stop can reuse a numeric frame ID, but cannot revive a
             * capture owned by the earlier stop and its retired registries. */
            CHECK(UmiDebugInstructionPageRead(platform, fixture.workspace, target, 0, 0, 2U, 0,
                                              100U, page) != UMI_STATUS_OK);
            UmiDebugInstructionTarget *fresh = NULL;
            OK(UmiDebugInstructionTargetCapture(platform, fixture.workspace, 2000U, &fresh));
            OK(UmiDebugInstructionPageRead(platform, fixture.workspace, fresh, 0, 0, 2U, 0, 2000U,
                                           page));
            UmiDebugInstructionTargetDestroy(fresh);
        }
    }
    char *log = NULL;
    size_t bytes = 0U;
    OK(umi_fs_read_text(transcript, &log, &bytes));
    CHECK(CommandCount(log, command) == (strcmp(name, "invalid") == 0 ? 0U : 1U));
    CHECK(CommandCount(log, "pause") ==
          ((strcmp(name, "pause") == 0 || strcmp(name, "pause-ack") == 0) ? 1U : 0U));
    CHECK(CommandCount(log, "evaluate") == 0U && CommandCount(log, "writeMemory") == 0U);
    umi_fs_free_text(log);
    UmiDebugInstructionTargetDestroy(target);
    free(page);
    free(unchanged);
    Close(&fixture);
    umi_debug_runtime_platform_destroy(platform);
    return 0;
}
