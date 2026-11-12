/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_paper_transaction.c
 *
 * PURPOSE:
 *   Verify paper state and audit publication, retained refusal reasons, self-pointer ownership and rollback of local candidate failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/paper_runtime.h"
#include "../trading/test_trading_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static void FillAudit(UmiPaperBrokerRuntime *runtime)
{
    while (runtime->audit.count < UMI_BROKER_AUDIT_CAPACITY) {
        UmiBrokerAuditRecord row = {0};
        row.sequence = runtime->nextAuditSequence;
        strcpy(row.eventId, "retained"); strcpy(row.source, "paper");
        strcpy(row.kind, "observation"); strcpy(row.entityId, "paper");
        row.eventTimeMilliseconds = 1100;
        CHECK(umi_broker_audit_journal_append(&runtime->audit, &row) == UMI_STATUS_OK);
        ++runtime->nextAuditSequence;
    }
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *operation = argv[1], *mode = argv[2];
    int connect = strcmp(operation, "connect") == 0, submit = strcmp(operation, "submit") == 0;
    int cancel = strcmp(operation, "cancel") == 0;
    if (!connect && !submit && !cancel) return 2;
    UmiPaperBrokerRuntime *runtime = malloc(sizeof(*runtime)), *before = malloc(sizeof(*before));
    CHECK(runtime != NULL && before != NULL);
    CHECK(umi_paper_broker_runtime_init(runtime) == UMI_STATUS_OK);
    if (!connect || strcmp(mode, "clock") == 0)
        CHECK(umi_paper_broker_runtime_connect(runtime, 1000U) == UMI_STATUS_OK);
    UmiOrderRequest request = test_order_request(); request.environment = UMI_TRADING_PAPER;
    strcpy(request.client_order_id.value, "paper-local");
    if (cancel || strcmp(mode, "duplicate") == 0)
        CHECK(umi_paper_broker_runtime_submit(runtime, &request, 1100U) == UMI_STATUS_OK);
    UmiFinancialId id = request.client_order_id;
    uint64_t now = 1200U;
    UmiStatus expected = UMI_STATUS_OK;
    int unchanged = 0, auditedRefusal = 0, repeat = 0;
    if (strcmp(mode, "audit-full") == 0) { FillAudit(runtime); expected = UMI_STATUS_CAPACITY_EXCEEDED; unchanged = 1; }
    else if (strcmp(mode, "audit-revision") == 0) { runtime->audit.revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; unchanged = 1; }
    else if (strcmp(mode, "audit-sequence") == 0) { runtime->nextAuditSequence = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; unchanged = 1; }
    else if (strcmp(mode, "audit-zero") == 0) { runtime->nextAuditSequence = 0U; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "audit-count") == 0) { runtime->audit.count = UMI_BROKER_AUDIT_CAPACITY + 1U; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "audit-gap") == 0) { ++runtime->audit.gapCount; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "time-range") == 0) { now = (uint64_t)INT64_MAX + 1U; expected = UMI_STATUS_INVALID_ARGUMENT; unchanged = 1; }
    else if (strcmp(mode, "clock") == 0) { now = 900U; expected = UMI_STATUS_INVALID_ARGUMENT; unchanged = 1; }
    else if (strcmp(mode, "callback") == 0) { runtime->broker.connect = NULL; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "copied-owner") == 0) { runtime->broker.instance = &before->storage; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "order-revision") == 0) { runtime->orders.revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; auditedRefusal = 1; }
    else if (strcmp(mode, "order-count") == 0) { runtime->orders.count = UMI_BROKER_ORDER_JOURNAL_CAPACITY + 1U; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "storage-count") == 0) { runtime->storage.orders.count = UMI_TRADING_MAX_ORDERS + 1U; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "session-last") == 0) { runtime->session.revision = UINT64_MAX - 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; auditedRefusal = 1; }
    else if (strcmp(mode, "invalid-id") == 0) { memset(id.value, 'x', sizeof(id.value)); expected = UMI_STATUS_INVALID_ARGUMENT; unchanged = 1; }
    else if (strcmp(mode, "invalid-request") == 0) { memset(request.client_order_id.value, 'x', sizeof(request.client_order_id.value)); expected = UMI_STATUS_INVALID_ARGUMENT; unchanged = 1; }
    else if (strcmp(mode, "live") == 0) { request.environment = UMI_TRADING_LIVE; expected = UMI_STATUS_PERMISSION_DENIED; unchanged = 1; }
    else if (strcmp(mode, "simulation") == 0) { request.environment = UMI_TRADING_SIMULATION; expected = UMI_STATUS_PERMISSION_DENIED; auditedRefusal = 1; }
    else if (strcmp(mode, "duplicate") == 0) { expected = UMI_STATUS_ALREADY_EXISTS; auditedRefusal = 1; }
    else if (strcmp(mode, "unknown") == 0) { strcpy(id.value, "unknown"); expected = UMI_STATUS_NOT_FOUND; auditedRefusal = 1; }
    else if (strcmp(mode, "record-sequence") == 0) { runtime->orders.records[0].providerSequence = UINT64_MAX; expected = UMI_STATUS_INVALID_STATE; auditedRefusal = 1; }
    else if (strcmp(mode, "record-status") == 0) { runtime->orders.records[0].status = UMI_ORDER_CANCELLED; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "record-request") == 0) { runtime->orders.records[0].request.quantity += 1.0; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "repeat") == 0) {
        CHECK(umi_paper_broker_runtime_cancel(runtime, &id, 1150U) == UMI_STATUS_OK); repeat = 1;
    } else if (strcmp(mode, "normal") != 0) return 2;
    memcpy(before, runtime, sizeof(*runtime));
    UmiStatus status = connect ? umi_paper_broker_runtime_connect(runtime, now) :
        submit ? umi_paper_broker_runtime_submit(runtime, &request, now) :
                 umi_paper_broker_runtime_cancel(runtime, &id, now);
    CHECK(status == expected);
    if (unchanged) CHECK(memcmp(before, runtime, sizeof(*runtime)) == 0);
    else {
        CHECK(runtime->audit.count == before->audit.count + 1U);
        CHECK(runtime->nextAuditSequence == before->nextAuditSequence + 1U);
        CHECK(runtime->audit.records[runtime->audit.count - 1U].status == expected);
        CHECK(runtime->broker.instance == &runtime->storage);
        CHECK(UmiBrokerAuditJournalValidate(&runtime->audit) == UMI_STATUS_OK);
        if (auditedRefusal || repeat) {
            CHECK(memcmp(&before->storage, &runtime->storage, sizeof(runtime->storage)) == 0);
            CHECK(memcmp(&before->orders, &runtime->orders, sizeof(runtime->orders)) == 0);
            CHECK(memcmp(&before->session, &runtime->session, sizeof(runtime->session)) == 0);
        } else if (connect) CHECK(runtime->storage.connected && runtime->session.state == UMI_BROKER_SESSION_AUTHENTICATED);
        else if (submit) CHECK(runtime->storage.orders.count == 1U && runtime->orders.count == 1U);
        else CHECK(runtime->storage.orders.orders[0].status == UMI_ORDER_CANCELLED &&
                   runtime->orders.records[0].status == UMI_ORDER_CANCELLED);
    }
    /* The negative owner-copy case deliberately corrupts the self-pointer.
     * Release only the fixture allocations; no production destroy callback is
     * invoked through a table this test intentionally made invalid. */
    free(before); free(runtime); return 0;
}
