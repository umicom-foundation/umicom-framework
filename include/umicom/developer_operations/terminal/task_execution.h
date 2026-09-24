/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/terminal/task_execution.h
 *
 * PURPOSE:
 *   Project canonical Task Execution state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_TERMINAL_TASK_EXECUTION_H
#define UMICOM_DEVELOPER_OPERATIONS_TERMINAL_TASK_EXECUTION_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateTerminalTaskExecution(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
