/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_debug.c
 *
 * PURPOSE:
 *   Run the shared local native debugger and report its first stopped frame.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"
#include "umicom/debug_runtime/stop_observation.h"
#include "umicom/platform/path.h"
#include "umicom/protocol/json_writer.h"
#include <stdio.h>
#include <string.h>

typedef struct DebugOptions {
    const char *source;
    const char *program;
    const char *kind;
    const char *adapter;
    const char *toolDirectory;
    const char *timeout;
} DebugOptions;

static void DebugUsage(FILE *stream)
{
    (void)fputs("Usage: umicom debug --program PATH [--kind gdb|lldb] [--source PATH]\n"
        "             [--adapter PATH] [--tool-dir PATH] [--timeout-ms 1..30000] [-- ARGUMENTS]\n"
        "Launches a local native executable, reports the first stopped stack frame as JSON, then terminates the session.\n"
        "The default debugger is gdb. Source defaults to the current directory. Remaining paths resolve from source.\n"
        "Use a debug build of an executable you intend to run. Arguments after -- go directly to the debugged program.\n",
        stream);
}

static const char **DebugOption(DebugOptions *options, const char *name)
{
    if (strcmp(name, "--source") == 0) return &options->source;
    if (strcmp(name, "--program") == 0) return &options->program;
    if (strcmp(name, "--kind") == 0) return &options->kind;
    if (strcmp(name, "--adapter") == 0) return &options->adapter;
    if (strcmp(name, "--tool-dir") == 0) return &options->toolDirectory;
    if (strcmp(name, "--timeout-ms") == 0) return &options->timeout;
    return NULL;
}

static uint32_t DebugTimeout(const char *value)
{
    if (value == NULL) return 5000U;
    uint32_t result = 0U;
    for (; *value != '\0'; ++value) {
        if (*value < '0' || *value > '9') return 0U;
        result = result * 10U + (uint32_t)(*value - '0');
        if (result > 30000U) return 0U;
    }
    return result;
}

static UmiStatus DebugPaths(const DebugOptions *options, char *source, char *program,
    char *adapter, char *toolDirectory)
{
    char cwd[UMI_PATH_CAPACITY];
    UmiStatus status = umi_fs_current_directory(cwd, sizeof(cwd));
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(options->source == NULL ? "." : options->source, cwd,
            source, UMI_PATH_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(options->program, source, program, UMI_PATH_CAPACITY);
    if (status == UMI_STATUS_OK && options->adapter != NULL)
        status = umi_path_absolute(options->adapter, source, adapter, UMI_PATH_CAPACITY);
    if (status == UMI_STATUS_OK && options->toolDirectory != NULL)
        status = umi_path_absolute(options->toolDirectory, source, toolDirectory, UMI_PATH_CAPACITY);
    return status;
}

static UmiStatus WriteStoppedFrame(const UmiDebugStackFrameSnapshot *frame)
{
    char output[8192];
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, output, sizeof(output));
    UmiJsonWriterRaw(&writer, "{\"event\":\"stopped\",\"frame\":");
    UmiJsonWriterString(&writer, frame->name);
    UmiJsonWriterRaw(&writer, ",\"source\":"); UmiJsonWriterString(&writer, frame->source_uri);
    UmiJsonWriterRaw(&writer, ",\"line\":"); UmiJsonWriterUInt64(&writer, frame->line);
    UmiJsonWriterRaw(&writer, ",\"column\":"); UmiJsonWriterUInt64(&writer, frame->column);
    UmiJsonWriterRaw(&writer, "}\n");
    if (writer.status != UMI_STATUS_OK) return writer.status;
    return fputs(output, stdout) == EOF || fflush(stdout) != 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

int umi_cli_command_debug(int argc, char **argv)
{
    if (argc == 1 && strcmp(argv[0], "--help") == 0) {
        DebugUsage(stdout);
        return 0;
    }
    DebugOptions options = {0};
    int index = 0;
    while (index < argc) {
        const char *name = argv[index++];
        if (strcmp(name, "--") == 0) break;
        const char **selection = DebugOption(&options, name);
        if (selection == NULL || *selection != NULL || index == argc ||
            argv[index][0] == '\0' || strncmp(argv[index], "--", 2U) == 0) {
            DebugUsage(stderr);
            return 2;
        }
        *selection = argv[index++];
    }
    const char *kind = options.kind == NULL ? "gdb" : options.kind;
    uint32_t timeout = DebugTimeout(options.timeout);
    if (options.program == NULL || timeout == 0U || (strcmp(kind, "gdb") != 0 && strcmp(kind, "lldb") != 0)) {
        DebugUsage(stderr);
        return 2;
    }

    char source[UMI_PATH_CAPACITY], program[UMI_PATH_CAPACITY];
    char adapter[UMI_PATH_CAPACITY] = {0}, toolDirectory[UMI_PATH_CAPACITY] = {0};
    UmiStatus status = DebugPaths(&options, source, program, adapter, toolDirectory);
    UmiDebugRuntimePlatform *platform = NULL;
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_platform_create(&platform);
    const char *stage = "prepare";
    if (status == UMI_STATUS_OK) {
        stage = "launch";
        status = UmiDebugRuntimePlatformLaunchArgumentsWithToolDirectory(platform, kind, adapter, program,
            source, (const char *const *)(argv + index), (size_t)(argc - index), timeout, toolDirectory);
    }
    UmiDebugStackFrameSnapshot frame;
    if (status == UMI_STATUS_OK) {
        stage = "observe";
        status = UmiDebugRuntimeObserveStop(platform, timeout, &frame);
        UmiStatus stopped = umi_debug_runtime_platform_stop(platform, 1, timeout);
        if (status == UMI_STATUS_OK) {
            stage = "terminate";
            status = stopped;
        }
    }
    umi_debug_runtime_platform_destroy(platform);
    if (status == UMI_STATUS_OK) {
        stage = "write";
        status = WriteStoppedFrame(&frame);
    }
    if (status != UMI_STATUS_OK) (void)fprintf(stderr, "debug %s failed: %s\n", stage, umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
