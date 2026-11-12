/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_workspace_admission.c
 *
 * PURPOSE:
 *   Verify workspace and market-row capacity refusal occurs before changing nested alerts, history, tape or selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#include "../../src/trading/trade_tape.c"
#include "../../src/trading/workspace.c"
#define OWNER_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto done; } } while (0)

int main(int argc, char **argv)
{
    static const char *operations[] = {"add","remove","quote","bar","depth","trade","state","filter","study","instrument","order","environment","refresh","alert-add","alert-remove","alert-disable","alert-ack"};
    static const char *kinds[] = {"ordinary","last","exhausted","market-last","market-exhausted"};
    if (argc != 2) return 2;
    size_t op = 17U, kind = 5U;
    for (size_t i = 0U; i < 17U; ++i) {
        size_t n = strlen(operations[i]);
        if (!strncmp(argv[1], operations[i], n) && argv[1][n] == ':') {
            op = i;
            for (size_t k = 0U; k < 5U; ++k) if (!strcmp(argv[1]+n+1U, kinds[k])) kind = k;
            break;
        }
    }
    if (op == 17U || kind == 5U || (kind >= 3U && (op < 2U || op > 6U))) return 2;
    int result = 0;
    UmiTradingWorkspace *w = NULL, *before = NULL;
    UmiTradingTradeTape *tapeBefore = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0,1.0e12,5000.0,1.0e12};
    UmiInstrument first = test_instrument(), second = first, third = first;
    strcpy(second.instrument_id.value, "synthetic-second");
    strcpy(third.instrument_id.value, "synthetic-third");
    OWNER_CHECK(umi_trading_workspace_create(&config, &w) == UMI_STATUS_OK);
    OWNER_CHECK(umi_trading_workspace_add_instrument(w, &first) == UMI_STATUS_OK);
    OWNER_CHECK(umi_trading_workspace_add_instrument(w, &second) == UMI_STATUS_OK);
    OWNER_CHECK(umi_trading_workspace_set_health(w,1,0,1) == UMI_STATUS_OK);
    UmiQuote quote = {0}; quote.instrument = first; quote.bid = 99; quote.ask = 101; quote.event_time_ms = 1000;
    OWNER_CHECK(umi_trading_workspace_update_quote(w,&quote) == UMI_STATUS_OK);
    UmiBar bar = {0}; bar.instrument=first; bar.open=100; bar.high=105; bar.low=95; bar.close=102; bar.volume=5; bar.start_time_ms=0; bar.end_time_ms=100;
    OWNER_CHECK(umi_trading_workspace_update_bar(w,&bar,99) == UMI_STATUS_OK);
    UmiMarketDepth depth = {0}; depth.instrument=first; depth.bid_count=1; depth.ask_count=1;
    depth.bids[0]=(UmiDepthLevel){99,5}; depth.asks[0]=(UmiDepthLevel){101,5};
    OWNER_CHECK(umi_trading_workspace_update_depth(w,&depth) == UMI_STATUS_OK);
    UmiTradeTick tick = {0}; tick.instrument=first; tick.price=100; tick.size=2; tick.event_time_ms=1000;
    UmiTradingTradeTapeRecord record;
    OWNER_CHECK(umi_trading_trade_tape_record_init(&record,1,&tick,UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED,"regular") == UMI_STATUS_OK);
    OWNER_CHECK(umi_trading_workspace_update_trade(w,&record) == UMI_STATUS_OK);
    OWNER_CHECK(umi_trading_workspace_add_price_alert(w,"alert-one",first.instrument_id.value,
        UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,101,1000) == UMI_STATUS_OK);
    quote.bid=101; quote.ask=103; quote.event_time_ms=2000;
    OWNER_CHECK(umi_trading_workspace_update_quote(w,&quote) == UMI_STATUS_OK);
    OWNER_CHECK(w->alerts.alerts[0].active);
    OWNER_CHECK(umi_trading_workspace_set_draft_prices(w,100,0) == UMI_STATUS_OK);
    UmiRiskDecision decision;
    OWNER_CHECK(umi_trading_workspace_submit_order(w,2000,&decision) == UMI_STATUS_OK && decision.allowed);
    quote.bid=103; quote.ask=105; quote.event_time_ms=3000;
    bar.start_time_ms=100; bar.end_time_ms=200; bar.close=104;
    depth.bids[0].price=103; depth.asks[0].price=105;
    record.sequence=3; record.trade.price=104; record.trade.event_time_ms=3000;
    w->revision=kind==1U?UINT64_MAX-1U:kind==2U?UINT64_MAX:42U;
    w->markets[0].revision=kind==3U?UINT64_MAX-1U:kind==4U?UINT64_MAX:20U;
    before=malloc(sizeof(*before)); tapeBefore=malloc(sizeof(*tapeBefore));
    OWNER_CHECK(before!=NULL && tapeBefore!=NULL);
    memcpy(before,w,sizeof(*before)); memcpy(tapeBefore,w->trade_tape,sizeof(*tapeBefore));
    UmiStatus status=UMI_STATUS_INVALID_ARGUMENT;
    switch(op) {
    case 0: status=umi_trading_workspace_add_instrument(w,&third); break;
    case 1: status=umi_trading_workspace_remove_instrument(w,first.instrument_id.value); break;
    case 2: status=umi_trading_workspace_update_quote(w,&quote); break;
    case 3: status=umi_trading_workspace_update_bar(w,&bar,102); break;
    case 4: status=umi_trading_workspace_update_depth(w,&depth); break;
    case 5: status=umi_trading_workspace_update_trade(w,&record); break;
    case 6: status=umi_trading_workspace_set_market_state(w,first.instrument_id.value,UMI_MARKET_PREOPEN); break;
    case 7: status=umi_trading_workspace_set_instrument_filter(w,"absent-instrument"); break;
    case 8: status=umi_trading_workspace_set_chart_study(w,UMI_TRADING_CHART_STUDY_DONCHIAN,7); break;
    case 9: status=umi_trading_workspace_select_instrument(w,second.instrument_id.value); break;
    case 10: status=umi_trading_workspace_select_order(w,w->oms.orders.orders[0].request.client_order_id.value); break;
    case 11: status=umi_trading_workspace_set_environment(w,UMI_TRADING_PAPER); break;
    case 12: status=umi_trading_workspace_refresh(w); break;
    case 13: status=umi_trading_workspace_add_price_alert(w,"alert-two",first.instrument_id.value,UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,110,3000); break;
    case 14: status=umi_trading_workspace_remove_price_alert(w,"alert-one"); break;
    case 15: status=umi_trading_workspace_set_price_alert_enabled(w,"alert-one",0); break;
    case 16: status=umi_trading_workspace_acknowledge_price_alert(w,"alert-one"); break;
    }
    if(kind==2U || kind==4U) {
        OWNER_CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        OWNER_CHECK(!memcmp(before,w,sizeof(*w)));
        OWNER_CHECK(!memcmp(tapeBefore,w->trade_tape,sizeof(*tapeBefore)));
    } else {
        OWNER_CHECK(status==UMI_STATUS_OK && w->revision==before->revision+1U);
        if(op>=2U && op<=6U) OWNER_CHECK(w->markets[0].revision==before->markets[0].revision+1U);
        switch(op) {
        case 0: OWNER_CHECK(w->market_count==3U); break;
        case 1: OWNER_CHECK(w->market_count==1U && w->alerts.count==0U && w->trade_tape->count==0U); break;
        case 2: OWNER_CHECK(w->markets[0].quote.bid==103 && w->alerts.alerts[0].last_value==104); break;
        case 3: OWNER_CHECK(w->bar_histories[0].count==2U && w->markets[0].bar.close==104); break;
        case 4: OWNER_CHECK(w->markets[0].depth.bids[0].price==103); break;
        case 5: OWNER_CHECK(w->markets[0].trade_sequence==3U && w->trade_tape->count==2U); break;
        case 6: OWNER_CHECK(w->markets[0].market_state==UMI_MARKET_PREOPEN); break;
        case 7: OWNER_CHECK(!strcmp(w->instrument_filter,"absent-instrument") && !w->selected_instrument_id[0]); break;
        case 8: OWNER_CHECK(w->chart_study==UMI_TRADING_CHART_STUDY_DONCHIAN && w->chart_study_period==7U); break;
        case 9: OWNER_CHECK(!strcmp(w->selected_instrument_id,second.instrument_id.value)); break;
        case 10: OWNER_CHECK(!strcmp(w->selected_order_id,w->oms.orders.orders[0].request.client_order_id.value)); break;
        case 11: OWNER_CHECK(w->environment==UMI_TRADING_PAPER && w->draft_order.environment==UMI_TRADING_PAPER); break;
        case 12: OWNER_CHECK(w->market_count==before->market_count && w->oms.orders.count==before->oms.orders.count); break;
        case 13: OWNER_CHECK(w->alerts.count==2U); break;
        case 14: OWNER_CHECK(w->alerts.count==0U); break;
        case 15: OWNER_CHECK(!w->alerts.alerts[0].enabled && !w->alerts.alerts[0].active); break;
        case 16: OWNER_CHECK(!w->alerts.alerts[0].active && w->alerts.alerts[0].acknowledged); break;
        }
    }
done:
    free(tapeBefore); free(before);
    if(w!=NULL) umi_trading_workspace_destroy(w);
    return result;
}
