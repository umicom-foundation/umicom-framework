/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/executor.h
 *
 * PURPOSE:
 *   Define the execution boundary for developer operations and provide a native
 *   process executor backed by the existing Umicom platform process service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The callback boundary lets a product execute work synchronously, on a worker
 * thread, inside a sandbox, through a remote agent, or with a test double while
 * the pipeline lifecycle remains unchanged.  The built-in process executor is
 * a useful default for command-line tools and background workers.
 */
#ifndef UMICOM_DEVELOPER_EXECUTOR_H
#define UMICOM_DEVELOPER_EXECUTOR_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/operation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_EXECUTION_API_VERSION 1U
#define UMI_DEVELOPER_EXECUTION_OUTPUT_CAPACITY 4096U

typedef struct UmiDeveloperExecutionResult {
    uint32_t struct_size;
    uint32_t api_version;
    int exit_code;
    int launched;
    int cancelled;
    int timed_out;
    int output_truncated;
    uint64_t duration_ms;
    char output[UMI_DEVELOPER_EXECUTION_OUTPUT_CAPACITY];
} UmiDeveloperExecutionResult;

typedef UmiStatus (*UmiDeveloperExecuteFunction)(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result);

typedef struct UmiDeveloperExecutor {
    uint32_t struct_size;
    uint32_t api_version;
    void *user_data;
    UmiDeveloperExecuteFunction execute;
} UmiDeveloperExecutor;

void umi_developer_executor_init(
    UmiDeveloperExecutor *executor,
    UmiDeveloperExecuteFunction execute,
    void *user_data);

UmiStatus umi_developer_process_execute(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
