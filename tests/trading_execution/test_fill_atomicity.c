/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_fill_atomicity.c
 * PURPOSE: Check replay, bounded stores and prospective fill calculations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

static int Stores(void)
{
    UmiExecutionStore *s=calloc(1,sizeof *s); UmiPositionBook *p=calloc(1,sizeof *p);
    UmiOrderStore *o=calloc(1,sizeof *o); CHECK(s&&p&&o);
    UmiFinancialId id={"execution"}; const UmiExecutionReport *r=NULL; UmiPosition *position=NULL;
    UmiOrder *order=NULL; UmiInstrument instrument=test_instrument();
    s->count=SIZE_MAX; p->count=SIZE_MAX; o->count=SIZE_MAX;
    CHECK(UmiExecutionStoreFind(s,&id,&r)==UMI_STATUS_INVALID_STATE && r==NULL);
    CHECK(umi_position_book_get(p,&instrument,0,&position)==UMI_STATUS_INVALID_STATE && position==NULL);
    CHECK(umi_order_store_find(o,"order",&order)==UMI_STATUS_INVALID_STATE && order==NULL);
    free(s);free(p);free(o);return 0;
}
static int Arithmetic(void)
{
    UmiPosition p={0}; p.instrument=test_instrument(); p.instrument.multiplier=1;
    CHECK(umi_position_apply_fill(&p,UMI_SIDE_BUY,1,DBL_MAX)==UMI_STATUS_OK);
    CHECK(umi_position_apply_fill(&p,UMI_SIDE_BUY,1,DBL_MAX)==UMI_STATUS_OK);
    CHECK(p.quantity==2 && p.average_price==DBL_MAX);
    UmiPosition before=p;
    CHECK(umi_position_apply_fill(&p,UMI_SIDE_SELL,2,1)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&p,&before,sizeof p)==0);
    CHECK(umi_position_apply_fill(&p,(UmiSide)77,1,1)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&p,&before,sizeof p)==0);
    p=(UmiPosition){0};p.instrument=test_instrument();
    CHECK(umi_position_apply_fill(&p,UMI_SIDE_SELL,2,100)==UMI_STATUS_OK);
    CHECK(umi_position_apply_fill(&p,UMI_SIDE_BUY,3,90)==UMI_STATUS_OK);
    CHECK(p.quantity==1 && p.average_price==90 && p.realised_pnl==400);
    return 0;
}
static int OrderArithmetic(void)
{
    UmiOrder o={0};o.request=test_order_request();o.request.quantity=2;o.status=UMI_ORDER_ACCEPTED;o.version=1;
    UmiExecutionReport r={0};r.client_order_id=o.request.client_order_id;
    strcpy(r.execution_id.value,"fill");r.fill_quantity=1;r.fill_price=DBL_MAX;
    CHECK(umi_order_apply_execution(&o,&r)==UMI_STATUS_OK);
    CHECK(umi_order_apply_execution(&o,&r)==UMI_STATUS_OK);
    CHECK(o.average_fill_price==DBL_MAX && o.status==UMI_ORDER_FILLED);
    o.status=UMI_ORDER_ACCEPTED;o.filled_quantity=0;o.version=UINT64_MAX;
    UmiOrder before=o;
    CHECK(umi_order_apply_execution(&o,&r)==UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&o,&before,sizeof o)==0);return 0;
}
static int Workspace(const char *test)
{
    UmiTradingWorkspace *w=NULL;UmiTradingWorkspaceConfig config=umi_trading_workspace_config_default();
    config.risk_limit=(UmiRiskLimit){1000,DBL_MAX,5000,DBL_MAX};
    UmiInstrument instrument=test_instrument();instrument.multiplier=1;
    UmiRiskDecision d;UmiTradingWorkspaceSnapshot a,b;UmiPosition position;
    UmiExecutionReport r={0};strcpy(r.execution_id.value,"fill-a");
    r.fill_quantity=1;r.fill_price=100;r.event_time_ms=1000;
    CHECK(umi_trading_workspace_create(&config,&w)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(w,&instrument)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_health(w,1,0,1)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_quantity(w,2)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_prices(w,100,0)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_submit_order(w,1000,&d)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_snapshot(w,&a)==UMI_STATUS_OK);
    strcpy(r.client_order_id.value,a.selected_order_id);
    CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_OK);
    CHECK(umi_trading_workspace_snapshot(w,&a)==UMI_STATUS_OK);
    if(strcmp(test,"workspace-replay")==0){
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK);
        CHECK(a.revision==b.revision && a.execution_count==b.execution_count);
        CHECK(umi_trading_workspace_position_at(w,0,&position)==UMI_STATUS_OK && position.quantity==1);
        r.fill_price=101;
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && a.revision==b.revision);
        r.fill_price=100;strcpy(r.execution_id.value,"fill-b");
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && b.execution_count==2);
        CHECK(umi_trading_workspace_position_at(w,0,&position)==UMI_STATUS_OK && position.quantity==2);
    }else if(strcmp(test,"workspace-invalid")==0){
        strcpy(r.execution_id.value,"fill-b");r.fill_quantity=INFINITY;
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && a.revision==b.revision);
        r.fill_quantity=2;
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK && a.revision==b.revision);
    }else if(strcmp(test,"workspace-overflow")==0){
        /* Finish the long position at an enormous but finite average, then
         * reject a close whose realised loss cannot be represented. */
        strcpy(r.execution_id.value,"fill-b");r.fill_price=DBL_MAX;
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_side(w,UMI_SIDE_BUY)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_quantity(w,2)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_submit_order(w,1001,&d)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK);
        strcpy(r.client_order_id.value,b.selected_order_id);strcpy(r.execution_id.value,"fill-c");r.fill_quantity=2;
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_side(w,UMI_SIDE_SELL)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_quantity(w,4)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_submit_order(w,1002,&d)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(w,&a)==UMI_STATUS_OK);
        strcpy(r.client_order_id.value,a.selected_order_id);strcpy(r.execution_id.value,"fill-d");r.fill_quantity=4;r.fill_price=1;
        CHECK(umi_trading_workspace_record_execution(w,&r)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_trading_workspace_snapshot(w,&b)==UMI_STATUS_OK);
        CHECK(a.revision==b.revision && a.execution_count==b.execution_count && a.position_count==b.position_count);
        CHECK(umi_trading_workspace_position_at(w,0,&position)==UMI_STATUS_OK && position.quantity==4);
    }else{umi_trading_workspace_destroy(w);return 2;}
    umi_trading_workspace_destroy(w);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strcmp(argv[1],"store-bounds")==0)return Stores();
    if(strcmp(argv[1],"position-arithmetic")==0)return Arithmetic();
    if(strcmp(argv[1],"order-arithmetic")==0)return OrderArithmetic();
    return Workspace(argv[1]);
}
