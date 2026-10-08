/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/execution_reconciler.h
 *
 * PURPOSE:
 *   Reconcile broker execution identities, exact duplicates, corrections and
 *   late reports using the canonical execution-report contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_EXECUTION_RECONCILER_H
#define UMICOM_BROKER_CONNECTIVITY_EXECUTION_RECONCILER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/execution_report.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_EXECUTION_CAPACITY 256U

typedef struct UmiBrokerExecutionReconciler {
    UmiExecutionReport reports[UMI_BROKER_EXECUTION_CAPACITY];
    size_t count;
    uint64_t duplicates;
    uint64_t corrections;
    uint64_t lateReports;
    int64_t latestEventTimeMilliseconds;
    uint64_t revision;
} UmiBrokerExecutionReconciler;

/* Initialise before use. This generic reconciler permits an explicit same-ID
 * replacement only within the same client order. Distinct provider correction
 * IDs require provider-specific mapping before using this contract.
 * Counters and revision never wrap; refusal leaves the entire owner unchanged.
 * The latest event time is a high-water mark, including accepted corrections;
 * lateReports counts newly seen executions older than that mark.
 * find returns a borrowed record, or NULL for invalid storage or a missing ID. */
void umi_broker_execution_reconciler_init(
    UmiBrokerExecutionReconciler *reconciler);
UmiStatus umi_broker_execution_reconciler_accept(
    UmiBrokerExecutionReconciler *reconciler,
    const UmiExecutionReport *report);
const UmiExecutionReport *umi_broker_execution_reconciler_find(
    const UmiBrokerExecutionReconciler *reconciler,
    const char *executionId);

#ifdef __cplusplus
}
#endif
#endif
