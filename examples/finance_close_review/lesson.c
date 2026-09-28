/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A matched trade is not a settled trade; balanced books still need controls.
 * Uses public service contracts only, with integer minor units and whole lots.
 *---------------------------------------------------------------------------*/
#include "lesson.h"
#include "umicom/finance_operations/close_review.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Practice {
    UmiDataServer *server;
    UmiFinanceOperations *operations;
    UmiFinanceOperationCommand command;
    UmiFinanceOperationReceipt receipt;
    uint64_t revision;
    unsigned request;
} Practice;
static UmiStatus Id(UmiFinancialId *id, const char *text) { return umi_financial_id_assign(id, text); }
static UmiStatus Begin(Practice *p, UmiFinanceOperationKind kind, const char *id, const char *actor)
{
    char request[40];
    UmiFinanceOperationCommandInit(&p->command);
    p->command.kind = kind; p->command.expectedRevision = p->revision;
    int n = snprintf(request, sizeof request, "practice.%u", ++p->request);
    if (n < 0 || (size_t)n >= sizeof request) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus s = Id(&p->command.requestId, request);
    if (s == UMI_STATUS_OK) s = Id(&p->command.id, id);
    if (s == UMI_STATUS_OK) s = Id(&p->command.actorId, actor);
    return s;
}
static UmiStatus Apply(Practice *p)
{
    UmiStatus s = UmiFinanceOperationsApply(p->operations, &p->command, &p->receipt);
    if (s == UMI_STATUS_OK) p->revision = p->receipt.revision;
    return s;
}
static UmiStatus Inspect(Practice *p, int verbose, size_t expected)
{
    UmiFinanceCloseReview *r = NULL;
    UmiStatus s = UmiFinanceCloseReviewCreate(p->operations, "september", &r);
    if (s != UMI_STATUS_OK) return s;
    UmiFinanceCloseReviewInfo info;
    s = UmiFinanceCloseReviewGetInfo(r, &info);
    if (s == UMI_STATUS_OK && info.issueCount != expected) s = UMI_STATUS_INTERNAL_ERROR;
    if (s == UMI_STATUS_OK && verbose) {
        size_t bytes = 0;
        s = UmiFinanceCloseReviewFormat(r, NULL, 0U, &bytes);
        if (s == UMI_STATUS_CAPACITY_EXCEEDED) {
            char *text = malloc(bytes);
            if (!text) s = UMI_STATUS_OUT_OF_MEMORY;
            else { s = UmiFinanceCloseReviewFormat(r, text, bytes, NULL); if (s == UMI_STATUS_OK) puts(text); free(text); }
        }
    }
    UmiFinanceCloseReviewDestroy(r);
    return s;
}
int UmiFinanceCloseLesson(int verbose)
{
    Practice p = {0};
    UmiStatus status = umi_data_server_create_memory(&p.server);
    if (status == UMI_STATUS_OK) status = UmiFinanceOperationsCreate(p.server, &p.operations);
    if (status != UMI_STATUS_OK) goto done;
#define STEP(call) do { status = (call); if (status != UMI_STATUS_OK) goto done; } while (0)
    const char *accounts[] = {"control", "buyer.cash", "seller.cash"};
    for (size_t i = 0; i < 3U; ++i) {
        STEP(Begin(&p, UMI_FINANCE_CREATE_ACCOUNT, accounts[i], "operator"));
        memcpy(p.command.name, "Workshop account", sizeof "Workshop account");
        p.command.currency = (UmiCurrency){{'G','B','P','\0'}}; p.command.scale = 2U;
        p.command.accountClass = i == 0U ? UMI_ACCOUNTING_ASSET : UMI_ACCOUNTING_LIABILITY;
        STEP(Apply(&p));
    }
    STEP(Begin(&p, UMI_FINANCE_OPEN_PERIOD, "september", "operator"));
    p.command.date = (UmiFinancialDate){2026,9U,1U}; p.command.endDate = (UmiFinancialDate){2026,9U,30U}; STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_ENTER_JOURNAL, "funding", "maker"));
    STEP(Id(&p.command.referenceId, "september")); p.command.date = (UmiFinancialDate){2026,9U,28U};
    p.command.lineCount = 2U; STEP(Id(&p.command.lines[0].accountId, "control"));
    p.command.lines[0].debitMinor = 50000;
    STEP(Id(&p.command.lines[1].accountId, "buyer.cash")); p.command.lines[1].creditMinor = 50000; STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_APPROVE_JOURNAL, "funding", "checker")); STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_POST_JOURNAL, "funding", "checker")); STEP(Apply(&p));
    for (size_t i = 0; i < 2U; ++i) {
        STEP(Begin(&p, UMI_FINANCE_REGISTER_PARTICIPANT, i ? "seller" : "buyer", "operator"));
        STEP(Id(&p.command.accountId, accounts[i + 1U]));
        memcpy(p.command.name, "Workshop participant", sizeof "Workshop participant"); STEP(Apply(&p));
    }
    STEP(Begin(&p, UMI_FINANCE_LIST_INSTRUMENT, "practice", "operator"));
    memcpy(p.command.name, "Workshop lots", sizeof "Workshop lots");
    p.command.currency = (UmiCurrency){{'G','B','P','\0'}}; p.command.scale = 2U;
    p.command.minorPerTick = 1; p.command.unitsPerLot = 1; p.command.maxOrderLots = 100; STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator")); p.command.enabled = true; STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_DEPOSIT_LOTS, "seller", "custodian"));
    STEP(Id(&p.command.instrumentId, "practice")); p.command.lots = 100; STEP(Apply(&p));
    for (size_t i = 0; i < 2U; ++i) {
        STEP(Begin(&p, UMI_FINANCE_PLACE_ORDER, i ? "buy.1" : "sell.1", "trader"));
        STEP(Id(&p.command.participantId, i ? "buyer" : "seller"));
        STEP(Id(&p.command.instrumentId, "practice")); p.command.date = (UmiFinancialDate){2026,9U,28U};
        p.command.side = i ? UMI_SIDE_BUY : UMI_SIDE_SELL;
        p.command.priceTicks = i ? 1100 : 1000; p.command.lots = i ? 6 : 10; STEP(Apply(&p));
    }
    STEP(Inspect(&p, verbose, 4U));
    puts("After matching: balanced ledger, but four close blockers remain.");
    STEP(Begin(&p, UMI_FINANCE_CLEAR_FILL, "system.fill.000001", "clearing")); STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_SETTLE_FILL, "system.fill.000001", "settlement"));
    STEP(Id(&p.command.periodId, "september")); p.command.date = (UmiFinancialDate){2026,9U,28U}; STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_CANCEL_ORDER, "sell.1", "trader"));
    STEP(Id(&p.command.participantId, "seller")); STEP(Apply(&p));
    const int64_t observations[] = {50000,44000,6000};
    for (size_t i = 0; i < 3U; ++i) {
        /* Fixed independent observations for this fictional exercise, not a
         * production practice of copying the ledger into reconciliation. */
        const char *ids[] = {"recon.control", "recon.buyer", "recon.seller"};
        STEP(Begin(&p, UMI_FINANCE_RECONCILE_ACCOUNT, ids[i], "reconciler"));
        STEP(Id(&p.command.accountId, accounts[i])); p.command.amountMinor = observations[i]; STEP(Apply(&p));
    }
    STEP(Inspect(&p, verbose, 0U));
    puts("After settlement, remainder cancellation and reconciliation: zero close blockers.");
    STEP(Begin(&p, UMI_FINANCE_PREPARE_CLOSE, "september", "preparer")); STEP(Apply(&p));
    STEP(Begin(&p, UMI_FINANCE_CLOSE_PERIOD, "september", "preparer"));
    if (Apply(&p) != UMI_STATUS_PERMISSION_DENIED) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    STEP(Begin(&p, UMI_FINANCE_CLOSE_PERIOD, "september", "closer")); STEP(Apply(&p));
    puts("A separate closer completes the period; the preparer cannot close it alone.");
    puts("Practice complete. Memory only; no broker connection or real settlement was made.");
#undef STEP
done:
    UmiFinanceOperationsDestroy(p.operations); umi_data_server_destroy(p.server);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Practice failed: status %d\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}
