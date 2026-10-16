/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/build_logging.c
 * PURPOSE: Keep complete development output in an explicitly selected private log.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_execution.h"
#include "umicom/platform/filesystem.h"
#include <string.h>

typedef struct BuildLogContext {
    UmiOutputFile *file;
    UmiBuildExecutionOptions original;
    UmiStatus execution_status;
} BuildLogContext;

/* The file owns raw bytes independently of the CLI's display. A failed sink
 * keeps its first error and never replaces an older log. */
static void capture_output(const char *bytes, size_t size, void *context)
{
    BuildLogContext *capture = context;
    (void)UmiOutputFileWrite(capture->file, bytes, size);
    if (capture->original.output)
        capture->original.output(bytes, size, capture->original.observer_context);
}

static void capture_command(UmiBuildAction action,
    const UmiProcessRequest *command, void *context)
{
    BuildLogContext *capture = context;
    if (capture->original.command)
        capture->original.command(action, command, capture->original.observer_context);
}

/* Finish the active child before reporting a storage failure. Returning the
 * error at this boundary prevents install/package after an incomplete build log. */
static UmiStatus capture_execute(const UmiProcessRequest *command,
    UmiProcessOutputObserver output, void *output_context,
    UmiProcessResult *result, void *context)
{
    BuildLogContext *capture = context;
    UmiStatus status = capture->original.executor
        ? capture->original.executor(command, output, output_context, result,
            capture->original.executor_context)
        : UmiProcessExecuteStreamed(command, output, output_context, result);
    capture->execution_status = status != UMI_STATUS_OK ? status :
        result->cancelled ? UMI_STATUS_CANCELLED :
        result->timed_out ? UMI_STATUS_TIMEOUT :
        result->exit_code != 0 ? UMI_STATUS_INTERNAL_ERROR : UMI_STATUS_OK;
    UmiOutputFileSnapshot snapshot;
    UmiStatus inspected = UmiOutputFileRead(capture->file, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (inspected != UMI_STATUS_OK) return inspected;
    return snapshot.status;
}

UmiStatus UmiBuildExecuteLogged(const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment, UmiBuildAction action,
    const UmiBuildRequest *request, const UmiBuildExecutionOptions *options,
    const char *log_path, UmiBuildExecutionLog *log, UmiBuildReport *report)
{
    if (!log || !report) return UMI_STATUS_INVALID_ARGUMENT;
    memset(log, 0, sizeof *log);
    umi_build_report_init(report);
    report->last_action = action;
    /* Check requests before creating evidence. Preview uses no storage. */
    UmiStatus status = umi_build_request_validate(action, request, NULL, 0U);
    if (!profile) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) {
        log->execution_status = report->last_status = status;
        return status;
    }
    if (!log_path || (options && options->preview)) {
        status = UmiBuildExecuteWithOptions(profile, environment, action, request, options, report);
        log->execution_status = status;
        return status;
    }
    /* A clean must not unlink its own log on platforms that allow deleting an
     * open file. Resolve the selected tree before reserving the output path. */
    if (request->clean || action == UMI_BUILD_CLEAN) {
        char cwd[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], binary[UMI_PATH_CAPACITY];
        status = umi_fs_current_directory(cwd, sizeof cwd);
        if (status == UMI_STATUS_OK) status = umi_path_absolute(request->source_root, cwd, source, sizeof source);
        if (status == UMI_STATUS_OK) status = umi_path_absolute(request->build_directory, source, binary, sizeof binary);
        if (status == UMI_STATUS_OK && umi_path_is_within(binary, log_path)) status = UMI_STATUS_INVALID_ARGUMENT;
        if (status != UMI_STATUS_OK) {
            log->log_status = report->last_status = status;
            log->execution_status = UMI_STATUS_UNAVAILABLE;
            return status;
        }
    }
    BuildLogContext capture = {0};
    if (options) capture.original = *options;
    status = UmiOutputFileCreate(log_path, &capture.file);
    if (status != UMI_STATUS_OK) {
        log->log_status = status;
        log->execution_status = UMI_STATUS_UNAVAILABLE;
        report->last_status = status;
        return status;
    }
    log->created = 1;
    UmiBuildExecutionOptions observed = capture.original;
    observed.output = capture_output;
    observed.command = capture_command;
    observed.observer_context = &capture;
    observed.executor = capture_execute;
    observed.executor_context = &capture;
    status = UmiBuildExecuteWithOptions(profile, environment, action, request, &observed, report);
    log->log_status = UmiOutputFileClose(capture.file);
    (void)UmiOutputFileRead(capture.file, &log->file);
    log->execution_status = log->log_status == UMI_STATUS_OK ? status : capture.execution_status;
    log->capture_complete = log->log_status == UMI_STATUS_OK && log->file.closed;
    UmiOutputFileDestroy(capture.file);
    return status != UMI_STATUS_OK ? status : log->log_status;
}
