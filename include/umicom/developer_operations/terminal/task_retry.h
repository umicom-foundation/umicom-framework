/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/terminal/task_retry.h
 *
 * PURPOSE:
 *   Project canonical Task Retry state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_TERMINAL_TASK_RETRY_H
#define UMICOM_DEVELOPER_OPERATIONS_TERMINAL_TASK_RETRY_H

#include <stddef.h>
#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateTerminalTaskRetry(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus UmiDeveloperOperationsEvaluateTerminalTaskRetryBatch(const UmiDeveloperOperationInput *inputs,
    size_t count, UmiDeveloperOperationSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
