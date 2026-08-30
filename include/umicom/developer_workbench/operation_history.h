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

typedef struct UmiDeveloperWorkbenchOperationHistory
    UmiDeveloperWorkbenchOperationHistory;

UmiStatus umi_developer_workbench_operation_history_create(
    UmiDeveloperWorkbenchOperationHistory **out_history);

void umi_developer_workbench_operation_history_destroy(
    UmiDeveloperWorkbenchOperationHistory *history);

UmiStatus umi_developer_workbench_operation_history_append(
    UmiDeveloperWorkbenchOperationHistory *history,
    const char *command_id,
    UmiBuildAction action,
    UmiStatus status,
    const UmiBuildReport *report);

UmiStatus umi_developer_workbench_operation_history_at(
    const UmiDeveloperWorkbenchOperationHistory *history,
    size_t index,
    UmiDeveloperWorkbenchOperationRecord *out_record);

size_t umi_developer_workbench_operation_history_count(
    const UmiDeveloperWorkbenchOperationHistory *history);

#ifdef __cplusplus
}
#endif

#endif
