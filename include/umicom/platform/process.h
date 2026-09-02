/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/process.h
 *
 * PURPOSE:
 *   Define safe argument-based child-process execution with explicit working
 *   directories, environment overrides, exit status, and captured output.
 *   This replaces product-specific shell command construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the environment variable data shared with callers of this public contract.
 */
typedef struct UmiEnvironmentVariable {
    const char *name;
    const char *value;
} UmiEnvironmentVariable;

/*
 * Child-process window policy.
 *
 * INHERIT keeps the platform's normal behaviour. HIDDEN is intended for
 * background probes, compilers, repository checks, and other processes whose
 * output is consumed by the application. VISIBLE remains available for tools
 * that deliberately own a native window.
 */
typedef enum UmiProcessWindowMode {
    UMI_PROCESS_WINDOW_INHERIT = 0,
    UMI_PROCESS_WINDOW_HIDDEN = 1,
    UMI_PROCESS_WINDOW_VISIBLE = 2
} UmiProcessWindowMode;

/**
 * Represent the process request data shared with callers of this public contract.
 */
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
    UmiProcessWindowMode window_mode;
} UmiProcessRequest;

/**
 * Represent the process result data shared with callers of this public contract.
 */
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

/**
 * Perform process through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_process_execute(const UmiProcessRequest *request,
                              UmiProcessResult *out_result);
/**
 * Provide the process capture operation used by this module and its client applications.
 */
UmiStatus umi_process_capture(const char *program,
                              const char *const *arguments,
                              size_t argument_count,
                              char *out_text,
                              size_t capacity,
                              int *out_exit_code);
/**
 * Perform process through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_process_run(const char *command, int *exit_code);

#ifdef __cplusplus
}
#endif

#endif
