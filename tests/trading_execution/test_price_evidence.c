/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_price_evidence.c
 * PURPOSE: Verify deterministic quote-backed risk without a live provider.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#include "umicom/trading_ui/trading_ui.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

static UmiQuote Quote(UmiInstrument instrument)
{
    UmiQuote q = {0}; q.instrument = instrument; q.bid = 99; q.ask = 101;
    q.bid_size = 10; q.ask_size = 12; q.event_time_ms = 1000; return q;
}

static int PriceCase(const char *test)
{
    UmiOrderRequest r = test_order_request(); UmiQuote q = Quote(r.instrument);
    UmiRiskLimit l = {10, 100000, 30, 10000};
    UmiRiskPricePolicy p = UmiRiskPricePolicyDefault();
    UmiPretradeRiskEvidence e; UmiRiskDecision d;
    r.type = UMI_ORDER_MARKET; r.limit_price = 0; r.quantity = 2;
    if (strcmp(test, "buy-sell") == 0) {
        d = UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e);
        CHECK(d.allowed && e.priceSource==UMI_RISK_PRICE_ASK);
        CHECK(e.referencePrice==101 && e.notional==4040 && e.quoteAgeMs==500);
        r.side=UMI_SIDE_SELL;
        d = UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e);
        CHECK(d.allowed && e.priceSource==UMI_RISK_PRICE_BID);
        CHECK(e.referencePrice==99 && e.notional==3960 && e.projectedPosition==-2);
    } else if (strcmp(test, "freshness") == 0) {
        CHECK(UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,6000,&p,&e).allowed);
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,6001,&p,&e).allowed);
        CHECK(!e.hasValuation);
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,999,&p,&e).allowed);
        q.event_time_ms=-1;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1000,&p,&e).allowed);
        q.event_time_ms=0;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,INT64_MAX,&p,&e).allowed);
    } else if (strcmp(test, "identity") == 0) {
        q.instrument.multiplier=1;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        q=Quote(r.instrument); q.instrument.expiry_yyyymmdd++;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        q=Quote(r.instrument); strcpy(q.instrument.venue,"OTHER");
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        q=Quote(r.instrument); memset(q.instrument.symbol,'x',sizeof q.instrument.symbol);
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
    } else if (strcmp(test, "prices") == 0) {
        q.ask=INFINITY;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        q=Quote(r.instrument); q.bid=102;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        q=Quote(r.instrument); q.ask_size=0;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        r.side=UMI_SIDE_SELL;
        CHECK(UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        q.bid_size=0;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
    } else if (strcmp(test, "limits-stops") == 0) {
        r.type=UMI_ORDER_LIMIT; r.limit_price=105;
        CHECK(UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,NULL,1500,&p,&e).allowed);
        CHECK(e.priceSource==UMI_RISK_PRICE_LIMIT && e.referencePrice==105);
        r.type=UMI_ORDER_STOP_LIMIT; r.stop_price=104;
        CHECK(UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,NULL,1500,&p,&e).allowed);
        r.type=UMI_ORDER_STOP; r.limit_price=0; r.stop_price=120;
        CHECK(UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        CHECK(e.priceSource==UMI_RISK_PRICE_STOP && e.referencePrice==120);
        r.stop_price=90;
        CHECK(UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        CHECK(e.referencePrice==101);
    } else if (strcmp(test, "policy-overflow") == 0) {
        p.priceBufferBps=100;
        CHECK(UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        CHECK(fabs(e.referencePrice-102.01)<0.00001);
        p.priceBufferBps=NAN; CHECK(!UmiRiskPricePolicyValid(&p));
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        p=UmiRiskPricePolicyDefault(); p.maxQuoteAgeMs=0; CHECK(!UmiRiskPricePolicyValid(&p));
        p=UmiRiskPricePolicyDefault(); q.bid=DBL_MAX; q.ask=DBL_MAX;
        l.max_order_notional=DBL_MAX;
        CHECK(!UmiPretradeRiskEvaluateQuoted(&r,&l,0,0,&q,1500,&p,&e).allowed);
        CHECK(!e.hasValuation);
    } else if (strcmp(test,"economic-order") == 0) {
        UmiOms *oms=calloc(1,sizeof *oms); CHECK(oms!=NULL); umi_oms_init(oms,l);
        r.limit_price=0.01; /* A stale UI field is not a MARKET price. */
        UmiOrderRequest before=r; l.max_order_notional=100; oms->risk_limit=l;
        CHECK(UmiOmsSubmitQuoted(oms,&r,0,0,1500,&q,&p,&d,&e)==UMI_STATUS_PERMISSION_DENIED);
        CHECK(!d.allowed && e.notional==4040 && oms->orders.count==0);
        oms->risk_limit.max_order_notional=100000;
        CHECK(UmiOmsSubmitQuoted(oms,&r,0,0,1501,&q,&p,&d,&e)==UMI_STATUS_OK);
        CHECK(oms->orders.orders[0].request.type==UMI_ORDER_MARKET);
        CHECK(oms->orders.orders[0].request.limit_price==0.01);
        CHECK(memcmp(&before,&r,sizeof r)==0); free(oms);
    } else return 2;
    return 0;
}

static int WorkspaceCase(const char *test)
{
    UmiTradingWorkspace *w=NULL; UmiInstrument instrument=test_instrument();
    UmiQuote q=Quote(instrument); UmiRiskDecision d; UmiPretradeRiskEvidence e;
    UmiTradingWorkspaceSnapshot a,b; UmiRiskPricePolicy policy=UmiRiskPricePolicyDefault();
    CHECK(umi_trading_workspace_create(NULL,&w)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(w,&instrument)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_update_quote(w,&q)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_health(w,1,0,1)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_type(w,UMI_ORDER_MARKET,UMI_TIF_DAY)==UMI_STATUS_OK);
    CHECK(UmiTradingWorkspacePreviewOrderAt(w,1500,&d)==UMI_STATUS_OK && d.allowed);
    CHECK(UmiTradingWorkspaceRiskEvidence(w,&e)==UMI_STATUS_OK && e.notional==2020);
    if (strcmp(test,"workspace-refresh") == 0) {
        q.bid=600000; q.ask=600001; q.event_time_ms=1600;
        CHECK(umi_trading_workspace_update_quote(w,&q)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_submit_order(w,1700,&d)==UMI_STATUS_PERMISSION_DENIED);
        CHECK(!d.allowed);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && b.order_count==0);
        CHECK(UmiTradingWorkspaceRiskEvidence(w,&e)==UMI_STATUS_OK && e.notional>10000000);
    } else if (strcmp(test,"workspace-expiry") == 0) {
        CHECK(umi_trading_workspace_submit_order(w,6001,&d)==UMI_STATUS_PERMISSION_DENIED);
        CHECK(!d.allowed);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && b.order_count==0);
    } else if (strcmp(test,"workspace-gates") == 0) {
        umi_trading_workspace_engage_kill_switch(w,"test");
        CHECK(umi_trading_workspace_submit_order(w,1500,&d)==UMI_STATUS_PERMISSION_DENIED);
        CHECK(!d.allowed); umi_trading_workspace_reset_kill_switch(w);
        CHECK(umi_trading_workspace_set_environment(w,UMI_TRADING_LIVE)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_submit_order(w,1500,&d)!=UMI_STATUS_OK);
        CHECK(!d.allowed);
    } else if (strcmp(test,"workspace-policy") == 0) {
        CHECK(umi_trading_workspace_snapshot(w,&a)==UMI_STATUS_OK);
        policy.priceBufferBps=NAN;
        CHECK(UmiTradingWorkspaceSetRiskPricePolicy(w,&policy)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && a.revision==b.revision);
        policy=UmiRiskPricePolicyDefault(); policy.maxQuoteAgeMs=100;
        CHECK(UmiTradingWorkspaceSetRiskPricePolicy(w,&policy)==UMI_STATUS_OK);
        CHECK(UmiTradingWorkspaceRiskEvidence(w,&e)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiTradingWorkspacePreviewOrderAt(w,1500,&d)==UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(test,"workspace-metadata") == 0) {
        CHECK(umi_trading_workspace_snapshot(w,&a)==UMI_STATUS_OK);
        q.instrument.multiplier=1;
        CHECK(umi_trading_workspace_update_quote(w,&q)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && a.revision==b.revision);
    } else {
        umi_trading_workspace_destroy(w); return 2;
    }
    umi_trading_workspace_destroy(w); return 0;
}

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strncmp(argv[1],"workspace-",10)==0) return WorkspaceCase(argv[1]);
    return PriceCase(argv[1]);
}
