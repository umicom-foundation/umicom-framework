/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/paper_runtime.h
 *
 * PURPOSE:
 *   Compose the canonical simulated broker with session, order and audit state
 *   as a provider-neutral paper-trading runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_PAPER_RUNTIME_H
#define UMICOM_BROKER_CONNECTIVITY_PAPER_RUNTIME_H

#include "umicom/trading/simulated_broker.h"
#include "umicom/broker_connectivity/session_supervisor.h"
#include "umicom/broker_connectivity/order_journal.h"
#include "umicom/broker_connectivity/audit_journal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPaperBrokerRuntime {
    UmiSimulatedBrokerStorage storage;
    UmiBroker broker;
    UmiBrokerSessionSupervisor session;
    UmiBrokerOrderJournal orders;
    UmiBrokerAuditJournal audit;
    uint64_t nextAuditSequence;
    int initialised;
} UmiPaperBrokerRuntime;

UmiStatus umi_paper_broker_runtime_init(UmiPaperBrokerRuntime *runtime);
UmiStatus umi_paper_broker_runtime_connect(
    UmiPaperBrokerRuntime *runtime,
    uint64_t nowMilliseconds);
UmiStatus umi_paper_broker_runtime_submit(
    UmiPaperBrokerRuntime *runtime,
    const UmiOrderRequest *request,
    uint64_t nowMilliseconds);
UmiStatus umi_paper_broker_runtime_cancel(
    UmiPaperBrokerRuntime *runtime,
    const UmiFinancialId *clientOrderId,
    uint64_t nowMilliseconds);
void umi_paper_broker_runtime_destroy(UmiPaperBrokerRuntime *runtime);

#ifdef __cplusplus
}
#endif
#endif
