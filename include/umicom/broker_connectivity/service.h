/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/service.h
 *
 * PURPOSE:
 *   Discover broker-connectivity policies and project provider/account/order/
 *   execution/position/audit state into one operator-facing platform snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_SERVICE_H
#define UMICOM_BROKER_CONNECTIVITY_SERVICE_H

#include <stddef.h>

#include "umicom/broker_connectivity/types.h"
#include "umicom/broker_connectivity/provider_registry.h"
#include "umicom/broker_connectivity/session_supervisor.h"
#include "umicom/broker_connectivity/account_store.h"
#include "umicom/broker_connectivity/order_journal.h"
#include "umicom/broker_connectivity/execution_reconciler.h"
#include "umicom/broker_connectivity/position_store.h"
#include "umicom/broker_connectivity/audit_journal.h"
#include "umicom/broker_connectivity/ibkr_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBrokerConnectivityPlatformSnapshot {
    uint64_t revision;
    size_t providerCount;
    size_t accountCount;
    size_t orderCount;
    size_t executionCount;
    size_t positionCount;
    size_t auditCount;

    uint64_t duplicateExecutions;
    uint64_t correctedExecutions;
    uint64_t lateExecutions;
    uint64_t auditGapCount;
    size_t persistedAuditCount;

    int connected;
    int authenticated;
    int paperReady;
    int liveReady;
    int ibkrConfigured;
    int accountReady;
    int executionReady;
    int positionReady;
    int auditReady;
} UmiBrokerConnectivityPlatformSnapshot;

size_t umi_broker_connectivity_service_count(void);

UmiStatus umi_broker_connectivity_service_id_at(
    size_t index,
    const char **outId);

UmiStatus umi_broker_connectivity_service_evaluate(
    const char *id,
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);

UmiStatus umi_broker_connectivity_platform_snapshot(
    const UmiBrokerProviderRegistry *providers,
    const UmiBrokerSessionSupervisor *session,
    const UmiBrokerAccountStore *accounts,
    const UmiBrokerOrderJournal *orders,
    const UmiBrokerExecutionReconciler *executions,
    const UmiBrokerPositionStore *positions,
    const UmiBrokerAuditJournal *audit,
    const UmiIbkrAdapterConfig *ibkr,
    int liveApproved,
    UmiBrokerConnectivityPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
