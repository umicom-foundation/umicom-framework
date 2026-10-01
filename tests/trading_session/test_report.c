/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_session/test_report.c
 * PURPOSE: Verify canonical session replay, filtering, currency separation and damaged evidence disclosure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include <float.h>
#include <math.h>
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];ReviewFixture f;ReviewFixtureInit(&f);
    UmiTradingSessionReport *report=NULL;UmiTradingSessionSummary summary;
    UmiTradingSessionSource *source=NULL;
    if(strcmp(name,"empty")==0){
        umi_trading_workspace_destroy(f.workspace);OK(umi_trading_workspace_create(NULL,&f.workspace));
        OK(UmiTradingSessionReportCapture(f.workspace,NULL,&report));OK(UmiTradingSessionReportSummary(report,&summary));
        CHECK(summary.orders==0&&summary.executions==0&&summary.positions==0&&summary.currencies==0&&summary.totalsAvailable);
    }else if(strcmp(name,"different-owner")==0){
        OK(UmiTradingSessionReportCapture(f.workspace,NULL,&report));
        umi_trading_workspace_destroy(f.workspace);f.workspace=NULL;
        ReviewFixture another;ReviewFixtureInit(&another);
        bool current=true;OK(UmiTradingSessionReportIsCurrent(report,another.workspace,&current));CHECK(!current);
        umi_trading_workspace_destroy(another.workspace);
    }else if(strcmp(name,"round-trip")==0){
        Fill(&f,f.first,"buy",2,100,1200);char sell[UMI_FINANCE_ID_CAPACITY],buy[UMI_FINANCE_ID_CAPACITY];
        Submit(&f,UMI_SIDE_SELL,3,110,sell);Fill(&f,sell,"sell",3,110,1100);
        Submit(&f,UMI_SIDE_BUY,1,105,buy);Fill(&f,buy,"cover",1,105,1000);
        OK(UmiTradingSessionReportCapture(f.workspace,"NQ",&report));OK(UmiTradingSessionReportSummary(report,&summary));
        CHECK(summary.issues==0&&summary.orders==3&&summary.executions==3&&summary.currencies==1);
        UmiPosition position;OK(UmiTradingSessionReportPositionAt(report,0,&position));CHECK(position.quantity==0&&position.average_price==0&&position.realised_pnl==25);
        UmiTradingSessionCurrency total;OK(UmiTradingSessionReportCurrencyAt(report,0,&total));CHECK(total.realisedPnl==25);
        UmiTradingSessionExecution e;OK(UmiTradingSessionReportExecutionAt(report,0,&e));CHECK(e.fill.event_time_ms==1200); /* Arrival, not timestamp order. */
    }else if(strcmp(name,"total-overflow")==0){
        source=Source(&f);source->orderCount=4;source->executionCount=4;source->positionCount=2;
        for(size_t i=0;i<2;++i){
            UmiOrder buy=source->orders[i];buy.request.quantity=1;buy.filled_quantity=1;buy.average_fill_price=1.0e307;buy.status=UMI_ORDER_FILLED;
            buy.request.limit_price=1.0e307;source->orders[i]=buy;
            UmiOrder sell=buy;sell.request.side=UMI_SIDE_SELL;sell.request.limit_price=1.1e308;sell.average_fill_price=1.1e308;
            (void)snprintf(sell.request.client_order_id.value,sizeof(sell.request.client_order_id.value),"large-sell-%zu",i);source->orders[2+i]=sell;
            source->executions[i]=ReviewFixtureFill(buy.request.client_order_id.value);
            (void)snprintf(source->executions[i].execution_id.value,sizeof(source->executions[i].execution_id.value),"large-buy-%zu",i);
            source->executions[i].fill_price=1.0e307;
            source->executions[2+i]=ReviewFixtureFill(sell.request.client_order_id.value);
            (void)snprintf(source->executions[2+i].execution_id.value,sizeof(source->executions[2+i].execution_id.value),"large-sell-%zu",i);
            source->executions[2+i].fill_price=1.1e308;
            source->positions[i]=(UmiPosition){0};source->positions[i].instrument=buy.request.instrument;
            source->positions[i].realised_pnl=1.1e308-1.0e307;
        }
        OK(UmiTradingBuildSessionReport(source,"",&report));HasIssue(report,UMI_SESSION_BOOK,UMI_SESSION_PNL);
        UmiCsvDocument *csv=NULL;OK(UmiTradingSessionReportExportCsv(report,&csv));CHECK(strstr(UmiCsvDocumentData(csv),"totals-withheld"));UmiCsvDocumentDestroy(csv);
    }else if(strcmp(name,"partial-cancel")==0){
        Fill(&f,f.first,"partial",1,100,1200);OK(umi_trading_workspace_select_order(f.workspace,f.first));
        OK(umi_trading_workspace_cancel_selected_order(f.workspace));OK(UmiTradingSessionReportCapture(f.workspace,NULL,&report));
        OK(UmiTradingSessionReportSummary(report,&summary));CHECK(summary.issues==0&&summary.executions==1);
    }else if(strcmp(name,"ownership")==0||strcmp(name,"current")==0||strcmp(name,"filters")==0){
        Fill(&f,f.first,"partial",1,100,1200);UmiTradingOrderQuery query={UMI_TRADING_WORKSPACE_ORDERS_ALL,"missing"};
        OK(UmiTradingWorkspaceSetOrderQuery(f.workspace,&query));UmiTradingWorkspaceSnapshot before,after;
        OK(umi_trading_workspace_snapshot(f.workspace,&before));OK(UmiTradingSessionReportCapture(f.workspace,"nQ",&report));
        OK(umi_trading_workspace_snapshot(f.workspace,&after));CHECK(before.revision==after.revision&&strcmp(before.selected_order_id,after.selected_order_id)==0);
        CHECK(memcmp(&before.draft_order,&after.draft_order,sizeof(before.draft_order))==0);
        OK(UmiTradingSessionReportSummary(report,&summary));CHECK(summary.retainedOrders==2&&summary.orders==1&&summary.executions==1&&summary.positions==1);
        bool current=false;OK(UmiTradingSessionReportIsCurrent(report,f.workspace,&current));CHECK(current);
        Fill(&f,f.first,"remaining",1,101,1100);OK(UmiTradingSessionReportIsCurrent(report,f.workspace,&current));CHECK(!current);
        if(strcmp(name,"ownership")==0){umi_trading_workspace_destroy(f.workspace);f.workspace=NULL;}
        OK(UmiTradingSessionReportSummary(report,&summary));CHECK(summary.executions==1);
        UmiTradingSessionExecution execution;OK(UmiTradingSessionReportExecutionAt(report,0,&execution));CHECK(execution.fill.fill_price==100);
    }else if(strcmp(name,"invalid")==0){
        CHECK(UmiTradingSessionReportCapture(NULL,"",&report)==UMI_STATUS_INVALID_ARGUMENT&&report==NULL);
        CHECK(UmiTradingSessionReportCapture(f.workspace,"",NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTradingSessionReportCapture(f.workspace,"bad\nfilter",&report)==UMI_STATUS_INVALID_ARGUMENT&&report==NULL);
        char huge[UMI_TRADING_WORKSPACE_FILTER_CAPACITY+1];memset(huge,'x',sizeof(huge));huge[sizeof(huge)-1]='\0';
        CHECK(UmiTradingSessionReportCapture(f.workspace,huge,&report)==UMI_STATUS_CAPACITY_EXCEEDED&&report==NULL);
        OK(UmiTradingSessionReportCapture(f.workspace,"",&report));UmiPosition position;memset(&position,1,sizeof(position));
        CHECK(UmiTradingSessionReportPositionAt(report,0,&position)==UMI_STATUS_NOT_FOUND&&position.instrument.instrument_id.value[0]=='\0');
        CHECK(UmiTradingSessionReportSummary(NULL,&summary)==UMI_STATUS_INVALID_ARGUMENT);
    }else{
        Fill(&f,f.first,"partial",1,100,1200);source=Source(&f);
        UmiTradingSessionArea area=UMI_SESSION_ORDER;uint32_t flags=0;
        if(strcmp(name,"quantity")==0){source->orders[0].filled_quantity=0.5;flags=UMI_SESSION_QUANTITY;}
        else if(strcmp(name,"price")==0){source->orders[0].average_fill_price=99;flags=UMI_SESSION_PRICE;}
        else if(strcmp(name,"status")==0){source->orders[0].status=UMI_ORDER_REJECTED;flags=UMI_SESSION_STATE;}
        else if(strcmp(name,"account")==0){strcpy(source->orders[0].request.account_id.value,"other");flags=UMI_SESSION_ACCOUNT;}
        else if(strcmp(name,"environment")==0){source->environment=UMI_TRADING_PAPER;flags=UMI_SESSION_ENVIRONMENT;}
        else if(strcmp(name,"duplicate-order")==0){source->orders[1]=source->orders[0];flags=UMI_SESSION_DUPLICATE;}
        else if(strcmp(name,"orphan-fill")==0){strcpy(source->executions[0].client_order_id.value,"unknown");area=UMI_SESSION_EXECUTION;flags=UMI_SESSION_MISSING;}
        else if(strcmp(name,"duplicate-fill")==0){source->executions[1]=source->executions[0];source->executionCount=2;area=UMI_SESSION_EXECUTION;flags=UMI_SESSION_DUPLICATE;}
        else if(strcmp(name,"position-quantity")==0){source->positions[0].quantity=2;area=UMI_SESSION_POSITION;flags=UMI_SESSION_QUANTITY;}
        else if(strcmp(name,"position-price")==0){source->positions[0].average_price=99;area=UMI_SESSION_POSITION;flags=UMI_SESSION_PRICE;}
        else if(strcmp(name,"position-pnl")==0){source->positions[0].realised_pnl=42;area=UMI_SESSION_POSITION;flags=UMI_SESSION_PNL;}
        else if(strcmp(name,"missing-position")==0){source->positionCount=0;area=UMI_SESSION_POSITION;flags=UMI_SESSION_MISSING;}
        else if(strcmp(name,"contract")==0){source->positions[0].instrument.multiplier=2;area=UMI_SESSION_POSITION;flags=UMI_SESSION_CONTRACT;}
        else if(strcmp(name,"currencies")==0){
            Fill(&f,f.second,"other-fill",1,100,1300);free(source);source=Source(&f);
            memcpy(source->orders[1].request.instrument.currency.code,"GBP",4);memcpy(source->positions[1].instrument.currency.code,"GBP",4);
        }else if(strcmp(name,"nonfinite")==0){source->positions[0].quantity=NAN;CHECK(UmiTradingBuildSessionReport(source,"",&report)==UMI_STATUS_INVALID_STATE&&report==NULL);goto cleanup;}
        else if(strcmp(name,"bounds")==0){source->executionCount=UMI_TRADING_MAX_ORDERS+1;CHECK(UmiTradingBuildSessionReport(source,"",&report)==UMI_STATUS_INVALID_STATE&&report==NULL);goto cleanup;}
        else CHECK(0);
        /* A narrow empty result cannot hide a damaged record from the audit. */
        OK(UmiTradingBuildSessionReport(source,flags?"no-such-instrument":"",&report));
        if(flags)HasIssue(report,area,flags);
        else {OK(UmiTradingSessionReportSummary(report,&summary));CHECK(summary.issues==0&&summary.currencies==2&&summary.positions==2);}
    }
cleanup:
    free(source);UmiTradingSessionReportDestroy(report);umi_trading_workspace_destroy(f.workspace);return 0;
}
