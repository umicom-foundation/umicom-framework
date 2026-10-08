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

/* Preserve the former find/accept operations for engineering review. The
 * replacement checks retained bounds and identity before reading records and
 * preflights every counter before mutation, preventing wraparound and a fill
 * being reassigned to another client order through a reused execution ID. */
#if 0
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

#endif

/* Keep the provider-neutral replacement contract for the same execution ID.
 * IBKR's distinct correction IDs are handled by its observation owner instead.
 * A correction cannot move a fill to another client order. */
static int ReconcilerKey(const char *text,size_t capacity)
{
    if(text==NULL) return 0;
    for(size_t i=0U;i<capacity;++i) if(text[i]=='\0') return i!=0U;
    return 0;
}
static int ReconcilerValid(const UmiBrokerExecutionReconciler *r)
{
    if(r==NULL || r->count>UMI_BROKER_EXECUTION_CAPACITY || r->revision==0U ||
        r->latestEventTimeMilliseconds<0) return 0;
    for(size_t i=0U;i<r->count;++i) {
        if(!umi_execution_report_valid(&r->reports[i])) return 0;
        for(size_t j=0U;j<i;++j)
            if(strcmp(r->reports[i].execution_id.value,r->reports[j].execution_id.value)==0)
                return 0;
    }
    return 1;
}
const UmiExecutionReport *umi_broker_execution_reconciler_find(
    const UmiBrokerExecutionReconciler *r,const char *executionId)
{
    if(!ReconcilerKey(executionId,sizeof r->reports[0].execution_id.value) || !ReconcilerValid(r)) return NULL;
    for(size_t i=0U;i<r->count;++i)
        if(strcmp(r->reports[i].execution_id.value,executionId)==0) return &r->reports[i];
    return NULL;
}
UmiStatus umi_broker_execution_reconciler_accept(UmiBrokerExecutionReconciler *r,const UmiExecutionReport *report)
{
    if(r==NULL || !umi_execution_report_valid(report)) return UMI_STATUS_INVALID_ARGUMENT;
    if(!ReconcilerValid(r)) return UMI_STATUS_INVALID_STATE;
    if(r->revision==UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for(size_t i=0U;i<r->count;++i) {
        if(strcmp(r->reports[i].execution_id.value,report->execution_id.value)!=0) continue;
        if(strcmp(r->reports[i].client_order_id.value,report->client_order_id.value)!=0)
            return UMI_STATUS_INVALID_STATE;
        if(UmiExecutionReportEqual(&r->reports[i],report)) {
            if(r->duplicates==UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
            ++r->duplicates;++r->revision;return UMI_STATUS_OK;
        }
        if(r->corrections==UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        r->reports[i]=*report;++r->corrections;++r->revision;
        if(report->event_time_ms>r->latestEventTimeMilliseconds)
            r->latestEventTimeMilliseconds=report->event_time_ms;
        return UMI_STATUS_OK;
    }
    if(r->count==UMI_BROKER_EXECUTION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    int late=r->count>0U && report->event_time_ms<r->latestEventTimeMilliseconds;
    if(late && r->lateReports==UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    r->reports[r->count++]=*report;if(late) ++r->lateReports;
    if(report->event_time_ms>r->latestEventTimeMilliseconds)
        r->latestEventTimeMilliseconds=report->event_time_ms;
    ++r->revision;return UMI_STATUS_OK;
}
