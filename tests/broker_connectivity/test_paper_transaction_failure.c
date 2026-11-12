/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_paper_transaction_failure.c
 *
 * PURPOSE:
 *   Inject allocation and audit publication failures to verify complete local rollback and usable retry state.
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
static int FailAllocation, FailAudit;
static void *FixtureAllocate(size_t size)
{
    return FailAllocation ? NULL : malloc(size);
}
static UmiStatus FixtureAppend(UmiBrokerAuditJournal *journal, const UmiBrokerAuditRecord *record)
{
    return FailAudit ? UMI_STATUS_IO_ERROR : umi_broker_audit_journal_append(journal, record);
}
UmiStatus FixturePaperInit(UmiPaperBrokerRuntime *);
UmiStatus FixturePaperConnect(UmiPaperBrokerRuntime *, uint64_t);
UmiStatus FixturePaperSubmit(UmiPaperBrokerRuntime *, const UmiOrderRequest *, uint64_t);
UmiStatus FixturePaperCancel(UmiPaperBrokerRuntime *, const UmiFinancialId *, uint64_t);
void FixturePaperDestroy(UmiPaperBrokerRuntime *);
/* Exercise production candidate/commit logic under deterministic ownership
 * failures. Only allocation and final audit admission are intercepted; simulator,
 * order journal and session behavior remain their canonical implementations. */
#define malloc FixtureAllocate
#define umi_broker_audit_journal_append FixtureAppend
#define umi_paper_broker_runtime_init FixturePaperInit
#define umi_paper_broker_runtime_connect FixturePaperConnect
#define umi_paper_broker_runtime_submit FixturePaperSubmit
#define umi_paper_broker_runtime_cancel FixturePaperCancel
#define umi_paper_broker_runtime_destroy FixturePaperDestroy
#include "../../src/broker_connectivity/paper_runtime.c"
#undef umi_paper_broker_runtime_destroy
#undef umi_paper_broker_runtime_cancel
#undef umi_paper_broker_runtime_submit
#undef umi_paper_broker_runtime_connect
#undef umi_paper_broker_runtime_init
#undef umi_broker_audit_journal_append
#undef malloc
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int connect = strcmp(argv[1], "connect") == 0, submit = strcmp(argv[1], "submit") == 0;
    int cancel = strcmp(argv[1], "cancel") == 0;
    if ((!connect && !submit && !cancel) ||
        (strcmp(argv[2], "allocation") != 0 && strcmp(argv[2], "audit") != 0)) return 2;
    UmiPaperBrokerRuntime *runtime = malloc(sizeof(*runtime)), *before = malloc(sizeof(*before));
    CHECK(runtime != NULL && before != NULL);
    CHECK(umi_paper_broker_runtime_init(runtime) == UMI_STATUS_OK);
    if (!connect) CHECK(umi_paper_broker_runtime_connect(runtime, 1000U) == UMI_STATUS_OK);
    UmiOrderRequest request = test_order_request(); request.environment = UMI_TRADING_PAPER;
    if (cancel) CHECK(umi_paper_broker_runtime_submit(runtime, &request, 1100U) == UMI_STATUS_OK);
    memcpy(before, runtime, sizeof(*runtime));
    FailAllocation = strcmp(argv[2], "allocation") == 0; FailAudit = !FailAllocation;
    UmiStatus status = connect ? FixturePaperConnect(runtime, 1200U) :
        submit ? FixturePaperSubmit(runtime, &request, 1200U) :
                 FixturePaperCancel(runtime, &request.client_order_id, 1200U);
    CHECK(status == (FailAllocation ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_IO_ERROR));
    CHECK(memcmp(before, runtime, sizeof(*runtime)) == 0);
    FailAllocation = FailAudit = 0;
    status = connect ? FixturePaperConnect(runtime, 1200U) :
        submit ? FixturePaperSubmit(runtime, &request, 1200U) :
                 FixturePaperCancel(runtime, &request.client_order_id, 1200U);
    CHECK(status == UMI_STATUS_OK && runtime->broker.instance == &runtime->storage);
    CHECK(runtime->audit.count == before->audit.count + 1U);
    umi_paper_broker_runtime_destroy(runtime);
    free(before); free(runtime);
    return 0;
}
