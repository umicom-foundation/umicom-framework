/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/execution_reconciler.c
 *
 * PURPOSE:
 *   Implement explicit duplicate, correction and late-execution reconciliation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/execution_reconciler.h"

#include <string.h>

void umi_broker_execution_reconciler_init(
    UmiBrokerExecutionReconciler *reconciler)
{
    if (reconciler == NULL) return;
    (void)memset(reconciler, 0, sizeof(*reconciler));
    reconciler->revision = 1U;
}

const UmiExecutionReport *umi_broker_execution_reconciler_find(
    const UmiBrokerExecutionReconciler *reconciler,
    const char *executionId)
{
    size_t index;
    if (reconciler == NULL || executionId == NULL) return NULL;
    for (index = 0U; index < reconciler->count; ++index) {
        if (strcmp(reconciler->reports[index].execution_id.value,
                   executionId) == 0) {
            return &reconciler->reports[index];
        }
    }
    return NULL;
}

UmiStatus umi_broker_execution_reconciler_accept(
    UmiBrokerExecutionReconciler *reconciler,
    const UmiExecutionReport *report)
{
    size_t index;

    if (reconciler == NULL || report == NULL ||
        !umi_execution_report_valid(report)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < reconciler->count; ++index) {
        if (strcmp(reconciler->reports[index].execution_id.value,
                   report->execution_id.value) == 0) {
            if (UmiExecutionReportEqual(&reconciler->reports[index], report)) {
                reconciler->duplicates += 1U;
                reconciler->revision += 1U;
                return UMI_STATUS_OK;
            }
            reconciler->reports[index] = *report;
            reconciler->corrections += 1U;
            reconciler->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    if (reconciler->count >= UMI_BROKER_EXECUTION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (reconciler->count > 0U &&
        report->event_time_ms < reconciler->latestEventTimeMilliseconds) {
        reconciler->lateReports += 1U;
    }
    reconciler->reports[reconciler->count++] = *report;
    if (report->event_time_ms > reconciler->latestEventTimeMilliseconds) {
        reconciler->latestEventTimeMilliseconds = report->event_time_ms;
    }
    reconciler->revision += 1U;
    return UMI_STATUS_OK;
}
