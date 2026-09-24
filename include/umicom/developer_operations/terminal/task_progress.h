/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/terminal/task_progress.h
 *
 * PURPOSE:
 *   Project canonical Task Progress state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_TERMINAL_TASK_PROGRESS_H
#define UMICOM_DEVELOPER_OPERATIONS_TERMINAL_TASK_PROGRESS_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateTerminalTaskProgress(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
