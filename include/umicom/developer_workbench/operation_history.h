/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/operation_history.h
 *
 * PURPOSE:
 *   Retain bounded configure/build/test/run/install/package evidence for
 *   dashboards, status bars, audit output and AI reasoning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_OPERATION_HISTORY_H
#define UMICOM_DEVELOPER_WORKBENCH_OPERATION_HISTORY_H

#include "umicom/developer_workbench/types.h"
#include "umicom/toolchain/build.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench operation record data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperWorkbenchOperationRecord {
    uint64_t sequence;
    char command_id[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    UmiBuildAction action;
    UmiStatus status;
    int exit_code;
    int cancelled;
    int timed_out;
    uint64_t duration_ms;
    char summary[UMI_DEVELOPER_WORKBENCH_TEXT_CAPACITY];
} UmiDeveloperWorkbenchOperationRecord;

/**
 * Represent the developer workbench operation history data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperWorkbenchOperationHistory
    UmiDeveloperWorkbenchOperationHistory;

/**
 * Initialise developer workbench operation history from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_operation_history_create(
    UmiDeveloperWorkbenchOperationHistory **out_history);

/**
 * Release or reset state held by developer workbench operation history so the same storage
 * can be reused safely.
 */
void umi_developer_workbench_operation_history_destroy(
    UmiDeveloperWorkbenchOperationHistory *history);

/**
 * Add developer workbench operation history only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_workbench_operation_history_append(
    UmiDeveloperWorkbenchOperationHistory *history,
    const char *command_id,
    UmiBuildAction action,
    UmiStatus status,
    const UmiBuildReport *report);

/**
 * Find developer workbench operation history while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_developer_workbench_operation_history_at(
    const UmiDeveloperWorkbenchOperationHistory *history,
    size_t index,
    UmiDeveloperWorkbenchOperationRecord *out_record);

/**
 * Return the number of records represented by developer workbench operation history
 * without changing their state.
 */
size_t umi_developer_workbench_operation_history_count(
    const UmiDeveloperWorkbenchOperationHistory *history);

#ifdef __cplusplus
}
#endif

#endif
