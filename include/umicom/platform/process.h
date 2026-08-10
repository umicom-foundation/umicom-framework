/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/process.h
 *
 * PURPOSE:
 *   Define safe argument-based child-process execution with explicit working
 *   directories, environment overrides, exit status, and captured output.
 *   This replaces product-specific shell command construction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_PROCESS_H
#define UMICOM_PLATFORM_PROCESS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROCESS_MAX_ARGUMENTS 64U
#define UMI_PROCESS_MAX_ENVIRONMENT 32U
#define UMI_PROCESS_OUTPUT_CAPACITY 65536U

typedef struct UmiEnvironmentVariable {
    const char *name;
    const char *value;
} UmiEnvironmentVariable;

typedef struct UmiProcessRequest {
    const char *program;
    const char *const *arguments;
    size_t argument_count;
    const char *working_directory;
    const UmiEnvironmentVariable *environment;
    size_t environment_count;
    int capture_stdout;
    int capture_stderr;
    uint32_t timeout_ms;
    uint32_t poll_interval_ms;
    const UmiCancellationToken *cancellation;
} UmiProcessRequest;

typedef struct UmiProcessResult {
    int exit_code;
    int launched;
    int output_truncated;
    int cancelled;
    int timed_out;
    int termination_requested;
    uint64_t duration_ms;
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
} UmiProcessResult;

UmiStatus umi_process_execute(const UmiProcessRequest *request,
                              UmiProcessResult *out_result);
UmiStatus umi_process_capture(const char *program,
                              const char *const *arguments,
                              size_t argument_count,
                              char *out_text,
                              size_t capacity,
                              int *out_exit_code);
UmiStatus umi_process_run(const char *command, int *exit_code);

#ifdef __cplusplus
}
#endif

#endif
