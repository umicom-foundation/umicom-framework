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
#include <stdlib.h>
#include "umicom/trading/order_request.h"
#include "umicom/finance/identifier.h"
#include <string.h>

/* Retain the original formatter for review. PaperFinish below increments the
 * sequence only after the complete candidate and audit row can be published. */
#if 0
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

#endif

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

/* Retain the direct-mutation operations for review. Their replacement stages
 * canonical local state and audit evidence together, avoiding partial order
 * publication when a later journal or audit operation refuses the update. */
#if 0
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

    /* Admission is shared with the canonical order service. Reject malformed
     * fixed fields before the adapter or audit formatter reads an identifier,
     * and leave all retained state unchanged when no order was attempted. */
    status = umi_order_request_validate(request);
    if (status != UMI_STATUS_OK) return status;

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

#endif

/* A paper runtime owns only the canonical local simulator. Stage the complete
 * operation on a heap candidate: neither an order nor a connection transition
 * becomes visible until its audit row has also been retained. This is not a
 * rollback mechanism for external broker callbacks or durable transactions. */
/* Compare the canonical serialized value rather than C structure padding.
 * The existing request owner defines fields and validation in one place.
 * A future larger schema is refused if it exceeds these bounded work buffers. */
static int PaperRequestSame(const UmiOrderRequest *left, const UmiOrderRequest *right)
{
    unsigned char a[2048], b[2048];
    size_t aSize = 0U, bSize = 0U;
    return umi_order_request_archive_encode(left, a, sizeof(a), &aSize) == UMI_STATUS_OK &&
        umi_order_request_archive_encode(right, b, sizeof(b), &bSize) == UMI_STATUS_OK &&
        aSize == bSize && memcmp(a, b, aSize) == 0;
}
static UmiStatus PaperCandidate(const UmiPaperBrokerRuntime *runtime, uint64_t now,
                                UmiPaperBrokerRuntime **out)
{
    if (now > (uint64_t)INT64_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiBrokerSessionSupervisorValidate(&runtime->session) != UMI_STATUS_OK ||
        UmiBrokerAuditJournalValidate(&runtime->audit) != UMI_STATUS_OK ||
        UmiBrokerOrderJournalValidate(&runtime->orders) != UMI_STATUS_OK ||
        runtime->storage.orders.count > UMI_TRADING_MAX_ORDERS ||
        runtime->storage.orders.count != runtime->orders.count ||
        (runtime->storage.connected != 0 && runtime->storage.connected != 1))
        return UMI_STATUS_INVALID_STATE;
    if (runtime->nextAuditSequence == 0U) return UMI_STATUS_INVALID_STATE;
    if (runtime->audit.count != 0U) {
        const UmiBrokerAuditRecord *last = &runtime->audit.records[runtime->audit.count - 1U];
        if (runtime->nextAuditSequence <= last->sequence) return UMI_STATUS_INVALID_STATE;
        if (now < (uint64_t)last->eventTimeMilliseconds) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (runtime->nextAuditSequence == UINT64_MAX ||
        runtime->audit.count == UMI_BROKER_AUDIT_CAPACITY ||
        runtime->audit.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < runtime->storage.orders.count; ++i) {
        const UmiOrder *order = &runtime->storage.orders.orders[i];
        if (umi_order_request_validate(&order->request) != UMI_STATUS_OK ||
            order->request.environment != UMI_TRADING_PAPER || order->version == 0U ||
            order->filled_quantity != 0.0 || order->average_fill_price != 0.0 ||
            (order->status != UMI_ORDER_ACCEPTED && order->status != UMI_ORDER_CANCELLED))
            return UMI_STATUS_INVALID_STATE;
        const UmiBrokerOrderRecord *row = umi_broker_order_journal_find(
            &runtime->orders, order->request.client_order_id.value);
        if (row == NULL || row->status != order->status ||
            !PaperRequestSame(&order->request, &row->request))
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(runtime->storage.orders.orders[j].request.client_order_id.value,
                       order->request.client_order_id.value) == 0) return UMI_STATUS_INVALID_STATE;
    }
    UmiPaperBrokerRuntime *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *runtime;
    UmiStatus status = umi_simulated_broker_create(&candidate->storage, &candidate->broker);
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    if (runtime->broker.instance != &runtime->storage ||
        runtime->broker.connect != candidate->broker.connect ||
        runtime->broker.submit_order != candidate->broker.submit_order ||
        runtime->broker.cancel_order != candidate->broker.cancel_order ||
        runtime->broker.destroy != candidate->broker.destroy) {
        free(candidate);
        return UMI_STATUS_INVALID_STATE;
    }
    candidate->storage = runtime->storage;
    *out = candidate;
    return UMI_STATUS_OK;
}
static UmiStatus PaperFinish(UmiPaperBrokerRuntime *runtime, UmiPaperBrokerRuntime *candidate,
                            const char *kind, const char *entity, uint64_t now, UmiStatus operation)
{
    if (operation != UMI_STATUS_OK) {
        /* Preserve the existing contract: admitted local attempts are audited
         * even when refused. Roll back staged operational state, keep the reason. */
        candidate->storage = runtime->storage;
        candidate->orders = runtime->orders;
        candidate->session = runtime->session;
    }
    UmiBrokerAuditRecord record = {0};
    record.sequence = candidate->nextAuditSequence;
    (void)snprintf(record.eventId, sizeof(record.eventId), "paper-%llu",
                   (unsigned long long)record.sequence);
    (void)snprintf(record.source, sizeof(record.source), "%s", "paper");
    (void)snprintf(record.kind, sizeof(record.kind), "%s", kind);
    (void)snprintf(record.entityId, sizeof(record.entityId), "%s", entity);
    record.eventTimeMilliseconds = (int64_t)now;
    record.status = operation;
    UmiStatus status = umi_broker_audit_journal_append(&candidate->audit, &record);
    if (status == UMI_STATUS_OK) {
        ++candidate->nextAuditSequence;
        runtime->storage = candidate->storage;
        runtime->orders = candidate->orders;
        runtime->session = candidate->session;
        runtime->audit = candidate->audit;
        runtime->nextAuditSequence = candidate->nextAuditSequence;
        /* The original broker table still points to runtime->storage. Never
         * publish the candidate's self-pointer, which dies with this allocation. */
    }
    free(candidate);
    return status == UMI_STATUS_OK ? operation : status;
}
UmiStatus umi_paper_broker_runtime_connect(UmiPaperBrokerRuntime *runtime, uint64_t now)
{
    if (runtime == NULL || !runtime->initialised) return UMI_STATUS_INVALID_STATE;
    UmiPaperBrokerRuntime *candidate = NULL;
    UmiStatus status = PaperCandidate(runtime, now, &candidate);
    if (status != UMI_STATUS_OK) return status;
    status = candidate->broker.connect(candidate->broker.instance, UMI_TRADING_PAPER);
    if (status == UMI_STATUS_OK)
        status = umi_broker_session_supervisor_connected(&candidate->session, now);
    if (status == UMI_STATUS_OK)
        status = umi_broker_session_supervisor_authenticated(&candidate->session, now);
    return PaperFinish(runtime, candidate, "connect", "paper", now, status);
}
UmiStatus umi_paper_broker_runtime_submit(UmiPaperBrokerRuntime *runtime,
                                        const UmiOrderRequest *request, uint64_t now)
{
    if (runtime == NULL || request == NULL || !runtime->initialised)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->session.state != UMI_BROKER_SESSION_AUTHENTICATED) return UMI_STATUS_UNAVAILABLE;
    if (request->environment == UMI_TRADING_LIVE) return UMI_STATUS_PERMISSION_DENIED;
    UmiStatus status = umi_order_request_validate(request);
    if (status != UMI_STATUS_OK) return status;
    UmiPaperBrokerRuntime *candidate = NULL;
    status = PaperCandidate(runtime, now, &candidate);
    if (status != UMI_STATUS_OK) return status;
    UmiOrderRequest captured = *request;
    status = candidate->broker.submit_order(candidate->broker.instance, &captured);
    if (status == UMI_STATUS_OK) {
        if (candidate->orders.revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else {
            UmiBrokerOrderRecord record = {0};
            record.request = captured;
            record.status = UMI_ORDER_ACCEPTED;
            record.providerSequence = candidate->orders.revision + 1U;
            record.submittedMilliseconds = record.updatedMilliseconds = now;
            status = umi_broker_order_journal_add(&candidate->orders, &record);
        }
    }
    return PaperFinish(runtime, candidate, "submit", captured.client_order_id.value, now, status);
}
UmiStatus umi_paper_broker_runtime_cancel(UmiPaperBrokerRuntime *runtime,
                                        const UmiFinancialId *clientOrderId, uint64_t now)
{
    if (runtime == NULL || clientOrderId == NULL || !runtime->initialised ||
        !umi_financial_id_valid(clientOrderId)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiPaperBrokerRuntime *candidate = NULL;
    UmiStatus status = PaperCandidate(runtime, now, &candidate);
    if (status != UMI_STATUS_OK) return status;
    UmiFinancialId captured = *clientOrderId;
    status = candidate->broker.cancel_order(candidate->broker.instance, &captured);
    if (status == UMI_STATUS_OK) {
        const UmiBrokerOrderRecord *previous = umi_broker_order_journal_find(
            &candidate->orders, captured.value);
        if (previous == NULL) status = UMI_STATUS_INVALID_STATE;
        else if (now < previous->updatedMilliseconds) status = UMI_STATUS_INVALID_ARGUMENT;
        else if (previous->status != UMI_ORDER_CANCELLED) {
            if (candidate->orders.revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
            else status = umi_broker_order_journal_update(&candidate->orders, captured.value,
                UMI_ORDER_CANCELLED, candidate->orders.revision + 1U, now);
        }
        /* A repeated local cancellation retains both order revisions. Its new
         * audit row acknowledges the attempt without inventing another change. */
    }
    return PaperFinish(runtime, candidate, "cancel", captured.value, now, status);
}

void umi_paper_broker_runtime_destroy(UmiPaperBrokerRuntime *runtime)
{
    if (runtime == NULL) return;
    if (runtime->initialised && runtime->broker.destroy != NULL) {
        runtime->broker.destroy(runtime->broker.instance);
    }
    (void)memset(runtime, 0, sizeof(*runtime));
}
