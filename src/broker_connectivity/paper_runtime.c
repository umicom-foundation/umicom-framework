/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/paper_runtime.c
 *
 * PURPOSE:
 *   Compose deterministic paper broker submission/cancellation with session,
 *   order journal and execution-control audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/paper_runtime.h"

#include <stdio.h>
#include <string.h>

static UmiStatus audit(
    UmiPaperBrokerRuntime *runtime,
    const char *kind,
    const char *entityId,
    uint64_t nowMilliseconds,
    UmiStatus status)
{
    UmiBrokerAuditRecord record = {0};
    record.sequence = runtime->nextAuditSequence++;
    (void)snprintf(record.eventId, sizeof(record.eventId),
                   "paper-%llu", (unsigned long long)record.sequence);
    (void)snprintf(record.source, sizeof(record.source), "%s", "paper");
    (void)snprintf(record.kind, sizeof(record.kind), "%s", kind);
    (void)snprintf(record.entityId, sizeof(record.entityId), "%s",
                   entityId != NULL ? entityId : "unknown");
    record.eventTimeMilliseconds = (int64_t)nowMilliseconds;
    record.status = status;
    return umi_broker_audit_journal_append(&runtime->audit, &record);
}

UmiStatus umi_paper_broker_runtime_init(UmiPaperBrokerRuntime *runtime)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(runtime, 0, sizeof(*runtime));
    umi_broker_session_supervisor_init(&runtime->session);
    umi_broker_order_journal_init(&runtime->orders);
    umi_broker_audit_journal_init(&runtime->audit);
    runtime->nextAuditSequence = 1U;
    status = umi_simulated_broker_create(&runtime->storage, &runtime->broker);
    if (status != UMI_STATUS_OK) return status;
    runtime->initialised = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_paper_broker_runtime_connect(
    UmiPaperBrokerRuntime *runtime,
    uint64_t nowMilliseconds)
{
    UmiStatus status;
    if (runtime == NULL || !runtime->initialised) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = runtime->broker.connect(
        runtime->broker.instance, UMI_TRADING_PAPER);
    if (status == UMI_STATUS_OK) {
        status = umi_broker_session_supervisor_connected(
            &runtime->session, nowMilliseconds);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_broker_session_supervisor_authenticated(
            &runtime->session, nowMilliseconds);
    }
    (void)audit(runtime, "connect", "paper", nowMilliseconds, status);
    return status;
}

UmiStatus umi_paper_broker_runtime_submit(
    UmiPaperBrokerRuntime *runtime,
    const UmiOrderRequest *request,
    uint64_t nowMilliseconds)
{
    UmiBrokerOrderRecord record = {0};
    UmiStatus status;

    if (runtime == NULL || request == NULL || !runtime->initialised) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (runtime->session.state != UMI_BROKER_SESSION_AUTHENTICATED) {
        return UMI_STATUS_UNAVAILABLE;
    }
    if (request->environment == UMI_TRADING_LIVE) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = runtime->broker.submit_order(runtime->broker.instance, request);
    if (status == UMI_STATUS_OK) {
        record.request = *request;
        record.status = UMI_ORDER_ACCEPTED;
        record.providerSequence = runtime->orders.revision + 1U;
        record.submittedMilliseconds = nowMilliseconds;
        record.updatedMilliseconds = nowMilliseconds;
        status = umi_broker_order_journal_add(&runtime->orders, &record);
    }
    (void)audit(runtime, "submit", request->client_order_id.value,
                nowMilliseconds, status);
    return status;
}

UmiStatus umi_paper_broker_runtime_cancel(
    UmiPaperBrokerRuntime *runtime,
    const UmiFinancialId *clientOrderId,
    uint64_t nowMilliseconds)
{
    UmiStatus status;

    if (runtime == NULL || clientOrderId == NULL || !runtime->initialised) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = runtime->broker.cancel_order(
        runtime->broker.instance, clientOrderId);
    if (status == UMI_STATUS_OK) {
        status = umi_broker_order_journal_update(
            &runtime->orders,
            clientOrderId->value,
            UMI_ORDER_CANCELLED,
            runtime->orders.revision + 1U,
            nowMilliseconds);
    }
    (void)audit(runtime, "cancel", clientOrderId->value,
                nowMilliseconds, status);
    return status;
}

void umi_paper_broker_runtime_destroy(UmiPaperBrokerRuntime *runtime)
{
    if (runtime == NULL) return;
    if (runtime->initialised && runtime->broker.destroy != NULL) {
        runtime->broker.destroy(runtime->broker.instance);
    }
    (void)memset(runtime, 0, sizeof(*runtime));
}
