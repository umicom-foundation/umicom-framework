/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_order_execution.c
 *
 * PURPOSE:
 *   Verify explicit authority, exact limit validation, single submission and uncertain delivery with fake transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

/* This fixture seeds completed synthetic account/metadata observations. Only
 * the in-memory transport can receive bytes; no broker or credentials exist. */
static int Ready(Fixture *f)
{
    CHECK(Connect(f) == 0);
    strcpy(f->c->snapshot.selectedAccount, "DU123");
    f->c->snapshot.summaryComplete = f->c->snapshot.positionsComplete = true;
    f->c->snapshot.summaryAtMilliseconds = f->c->snapshot.positionsAtMilliseconds = 5U;
    UmiIbkrContractDetailsSnapshot *d = &f->c->contractDetails;
    d->complete = true; d->count = 1U; d->completedAtMilliseconds = 5U;
    d->requestedContract.contractId = 123U; strcpy(d->requestedContract.exchange, "SMART");
    UmiIbkrContractDescription *r = &d->items[0]; r->contractId = 123U;
    strcpy(r->symbol, "EXAMPLE"); strcpy(r->securityType, "STK");
    strcpy(r->strike, "0"); strcpy(r->exchange, "SMART");
    strcpy(r->currency, "USD"); strcpy(r->localSymbol, "EXAMPLE");
    strcpy(r->tradingClass, "EXAMPLE"); strcpy(r->primaryExchange, "NYSE");
    strcpy(r->minimumTick, "0.01"); strcpy(r->validExchanges, "SMART,NYSE");
    strcpy(r->orderTypes, "LMT,MKT"); strcpy(r->minimumSize, "1"); strcpy(r->sizeIncrement, "1");
    CHECK(UmiIbkrExecutionEnable(f->c, "DU123", f->c->options.environment, true) == UMI_STATUS_OK);
    f->c->txSize = 0U;
    return 0;
}
static UmiIbkrOrderDraft Draft(void)
{
    UmiIbkrOrderDraft d = {0}; d.contractId = 123U; d.quantity = 2U;
    strcpy(d.account,"DU123"); strcpy(d.exchange,"SMART"); strcpy(d.limitPrice,"25.50");
    return d;
}
static int CheckWire(Fixture *f, int version)
{
    const char *v[128]; size_t count = 0U, at = 4U;
    CHECK(f->c->txSize > 4U);
    uint32_t size = ((uint32_t)f->c->tx[0] << 24U) | ((uint32_t)f->c->tx[1] << 16U) |
        ((uint32_t)f->c->tx[2] << 8U) | f->c->tx[3];
    CHECK(size == f->c->txSize - 4U);
    while (at < f->c->txSize) {
        CHECK(count < 128U); v[count++] = (const char *)f->c->tx + at;
        while (at < f->c->txSize && f->c->tx[at]) ++at;
        CHECK(at < f->c->txSize); ++at;
    }
    /* Frozen field positions from the negotiated single-contract LMT schema,
     * independent of the encoder's groups. A new protocol needs new evidence. */
    CHECK(count == (version < 166 ? 113U : version < 169 ? 114U : 115U));
    CHECK(!strcmp(v[0],"3") && !strcmp(v[1],"42") && !strcmp(v[2],"123"));
    CHECK(!strcmp(v[3],"EXAMPLE") && !strcmp(v[4],"STK") && !strcmp(v[9],"SMART"));
    CHECK(!strcmp(v[16],"BUY") && !strcmp(v[17],"2") && !strcmp(v[18],"LMT"));
    CHECK(!strcmp(v[19],"25.50") && !strcmp(v[20],"") && !strcmp(v[21],"DAY"));
    CHECK(!strcmp(v[23],"DU123") && !strcmp(v[27],"1") && !strcmp(v[33],"0"));
    CHECK(!strcmp(v[46],"-1") && !strcmp(v[85],"0"));
    CHECK(!strcmp(v[92],"1.7976931348623e+308") && !strcmp(v[112],"0"));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *f = New(); CHECK(f); CHECK(Ready(f) == 0);
    UmiIbkrOrderDraft d = Draft(); UmiIbkrOrderReview r = {0}; uint32_t id = 777U;
    const char *test = argv[1];
    if (!strcmp(test,"authority")) {
        UmiIbkrExecutionDisable(f->c);
        CHECK(f->c->snapshot.readOnly);
        CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiIbkrExecutionEnable(f->c,"DU123",f->c->options.environment,false) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiIbkrExecutionEnable(f->c,"DU456",f->c->options.environment,true) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiIbkrExecutionEnable(f->c,"DU123",UMI_TRADING_LIVE,true) == UMI_STATUS_PERMISSION_DENIED);
    } else if (!strcmp(test,"completion")) {
        UmiIbkrExecutionDisable(f->c); f->c->snapshot.positionsComplete = false;
        CHECK(UmiIbkrExecutionEnable(f->c,"DU123",f->c->options.environment,true) == UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(test,"invalid-price")) {
        const char *bad[] = {"0","-1","25.501","1e2","25,50","1.0000000001","nan","1.",""};
        for (size_t i=0U;i<sizeof bad/sizeof bad[0];++i) {
            strcpy(d.limitPrice,bad[i]);
            CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (!strcmp(test,"invalid-quantity")) {
        d.quantity=0U; CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_INVALID_ARGUMENT);
        d.quantity=1000001U; CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(test,"stale-metadata")) {
        CHECK(UmiIbkrOrderPrepare(f->c,&d,60006U,&r) == UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(test,"stale-account")) {
        f->c->contractDetails.completedAtMilliseconds=60006U;
        CHECK(UmiIbkrOrderPrepare(f->c,&d,60006U,&r)==UMI_STATUS_INVALID_STATE);
        CHECK(!f->c->txSize);
    } else if (!strcmp(test,"account-expires")) {
        f->c->contractDetails.completedAtMilliseconds=59999U;
        CHECK(UmiIbkrOrderPrepare(f->c,&d,59999U,&r)==UMI_STATUS_OK);
        CHECK(UmiIbkrOrderSubmit(f->c,r.token,60006U,&id)==UMI_STATUS_INVALID_STATE);
        CHECK(!f->c->txSize && id==777U);
    } else if (!strcmp(test,"ambiguous")) {
        f->c->contractDetails.count=2U;
        CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(test,"unsupported-bond")) {
        strcpy(f->c->contractDetails.items[0].securityType,"BOND");
        CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_NOT_IMPLEMENTED);
    } else if (!strcmp(test,"route")) {
        strcpy(d.exchange,"NYSE");
        CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(test,"edit-invalidates")) {
        CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_OK);
        d.quantity=0U; UmiIbkrOrderReview second={0};
        CHECK(UmiIbkrOrderPrepare(f->c,&d,7U,&second) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrOrderSubmit(f->c,r.token,8U,&id) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(id==777U && !f->c->txSize);
    } else {
        if (!strncmp(test,"wire-",5U)) f->c->snapshot.protocolVersion=atoi(test+5);
        if (!strcmp(test,"namespace")) f->c->orders.identity.conservativeNextOrderId=35001U;
        CHECK(UmiIbkrOrderPrepare(f->c,&d,6U,&r) == UMI_STATUS_OK);
        if (!strcmp(test,"expired")) {
            CHECK(UmiIbkrOrderSubmit(f->c,r.token,30007U,&id) == UMI_STATUS_INVALID_STATE);
            CHECK(id==777U && !f->c->txSize);
        } else if (!strcmp(test,"queue-full")) {
            f->c->txSize=sizeof f->c->tx;
            CHECK(UmiIbkrOrderSubmit(f->c,r.token,7U,&id) == UMI_STATUS_BUSY);
            CHECK(id==777U && !f->c->submission.report.count);
            CHECK(f->c->submission.report.reviewPending && f->c->orders.identity.conservativeNextOrderId==42U);
        } else {
            CHECK(UmiIbkrOrderSubmit(f->c,r.token,7U,&id) == UMI_STATUS_OK);
            CHECK(id==(!strcmp(test,"namespace")?36000U:42U));
            size_t submitted=f->c->txSize;
            CHECK(!f->c->submission.report.reviewPending && f->c->submission.report.count==1U);
            CHECK(UmiIbkrOrderSubmit(f->c,r.token,8U,&id) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(f->c->txSize==submitted);
            if (!strncmp(test,"wire-",5U)) CHECK(CheckWire(f,f->c->snapshot.protocolVersion)==0);
            else if (!strcmp(test,"cancel")) {
                UmiIbkrExecutionDisable(f->c);
                CHECK(UmiIbkrOrderCancelSubmitted(f->c,id,8U)==UMI_STATUS_OK);
                CHECK(UmiIbkrOrderCancelSubmitted(f->c,id,9U)==UMI_STATUS_INVALID_STATE);
                CHECK(UmiIbkrOrderCancelSubmitted(f->c,999U,9U)==UMI_STATUS_NOT_FOUND);
                CHECK(!strcmp((const char *)f->c->tx+submitted+4U,"4"));
            } else if (!strcmp(test,"disconnect")) {
                UmiIbkrSubmissionClosed(f->c);
                CHECK(!f->c->submission.report.enabled && f->c->submission.report.orders[0].uncertain);
                CHECK(UmiIbkrExecutionEnable(f->c,"DU123",f->c->options.environment,true)==UMI_STATUS_INVALID_STATE);
                CHECK(f->c->txSize==submitted);
            } else if (!strcmp(test,"rejection")) {
                CHECK(UmiIbkrSubmissionMessage(f->c,"42",201,"Synthetic refusal"));
                CHECK(f->c->submission.report.orders[0].rejected);
                CHECK(UmiIbkrOrderCancelSubmitted(f->c,42U,8U)==UMI_STATUS_INVALID_STATE);
            } else if (!strcmp(test,"read-queue")) {
                const char *fields[]={"3","42"};
                CHECK(UmiIbkrQueueFields(f->c,fields,2U)==UMI_STATUS_PERMISSION_DENIED);
                CHECK(f->c->txSize==submitted);
            } else if (!strcmp(test,"reconcile") || !strcmp(test,"foreign-client") ||
                       !strcmp(test,"identity-mismatch") || !strcmp(test,"status-only") ||
                       !strcmp(test,"terminal-sticky")) {
                UmiIbkrSubmissionClosed(f->c);
                UmiIbkrRecoveredOrder observed={0}; observed.key.orderId=42;
                observed.key.clientId=f->c->options.adapter.clientId;
                observed.hasOpenOrder=true; observed.open.contractId=d.contractId;
                strcpy(observed.open.account,d.account);
                if (!strcmp(test,"foreign-client")) ++observed.key.clientId;
                if (!strcmp(test,"identity-mismatch")) ++observed.open.contractId;
                if (!strcmp(test,"status-only")) {
                    observed.hasOpenOrder=false; observed.hasStatus=true;
                    strcpy(observed.status.status,"Submitted");
                }
                UmiIbkrSubmissionObserve(f->c,&observed);
                bool reconciled=!strcmp(test,"reconcile") || !strcmp(test,"terminal-sticky");
                CHECK(f->c->submission.report.orders[0].uncertain != reconciled);
                CHECK(!f->c->submission.report.enabled && f->c->txSize==submitted);
                CHECK(UmiIbkrExecutionEnable(f->c,"DU123",f->c->options.environment,true)==
                    (reconciled?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE));
                if (!strcmp(test,"terminal-sticky")) {
                    observed.hasStatus=true; strcpy(observed.status.status,"Filled");
                    UmiIbkrSubmissionObserve(f->c,&observed);
                    strcpy(observed.status.status,"Submitted");
                    UmiIbkrSubmissionObserve(f->c,&observed);
                    CHECK(f->c->submission.report.orders[0].filled);
                    CHECK(UmiIbkrOrderCancelSubmitted(f->c,42U,8U)==UMI_STATUS_INVALID_STATE);
                }
            } else if (strcmp(test,"single-use") && strcmp(test,"namespace")) { Delete(f); return 2; }
        }
    }
    Delete(f); return 0;
}
