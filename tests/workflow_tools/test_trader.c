/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workflow_tools/test_trader.c
 * PURPOSE:
 *   Prove ticket reset never submits an order or clears execution safeguards.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading_ui/trading_ui.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

typedef struct Fixture { UmiTradingWorkspace *workspace; UmiTradingUiController controller;
    UmiTradingSimulationMarket simulation; unsigned notifications; } Fixture;
static void Changed(uint64_t revision,void *context) { (void)revision; ((Fixture*)context)->notifications++; }
static int Setup(Fixture *f,int seed) {
    UmiTradingWorkspaceConfig config=umi_trading_workspace_config_default();
    UmiTradingUiControllerConfig ui=umi_trading_ui_controller_config_default();
    CHECK(umi_trading_workspace_create(&config,&f->workspace)==UMI_STATUS_OK);
    CHECK(umi_trading_ui_controller_init(&f->controller,f->workspace,&ui)==UMI_STATUS_OK);
    umi_trading_ui_controller_set_changed_handler(&f->controller,Changed,f);
    CHECK(umi_trading_simulation_market_init(&f->simulation,f->workspace)==UMI_STATUS_OK);
    if(seed)CHECK(umi_trading_simulation_market_seed_default(&f->simulation,1000000)==UMI_STATUS_OK);
    return 0;
}
static int Run(Fixture *f,const char *name) {
    UmiTradingWorkspaceSnapshot before,after;
    if(strcmp(name,"mapping")==0) {
        CHECK(umi_trading_ui_action_kind(UMI_TRADING_UI_ACTION_RESET_DRAFT)==UMI_TRADING_UI_ACTION_KIND_RESET_DRAFT);
        CHECK(strcmp(umi_trading_ui_action_id(UMI_TRADING_UI_ACTION_KIND_RESET_DRAFT),UMI_TRADING_UI_ACTION_RESET_DRAFT)==0);
        CHECK(!umi_trading_ui_action_requires_payload(UMI_TRADING_UI_ACTION_KIND_RESET_DRAFT));
    } else if(strcmp(name,"invalid")==0) {
        CHECK(UmiTradingWorkspaceResetDraft(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTradingUiControllerResetDraft(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else {
        if(strcmp(name,"empty")!=0)CHECK(umi_trading_workspace_select_instrument(f->workspace,"SIM.XAUUSD")==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_side(f->workspace,UMI_SIDE_SELL)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_type(f->workspace,UMI_ORDER_LIMIT,UMI_TIF_GTC)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_quantity(f->workspace,5.0)==UMI_STATUS_OK);
        CHECK(umi_trading_workspace_set_draft_prices(f->workspace,3396.0,123.0)==UMI_STATUS_OK);
        if(strcmp(name,"safeguards")==0) {
            umi_trading_workspace_engage_kill_switch(f->workspace,"keep stop");
        } else if(strcmp(name,"filters")==0) {
            CHECK(umi_trading_workspace_set_instrument_filter(f->workspace,"XAU")==UMI_STATUS_OK);
            CHECK(umi_trading_workspace_set_order_filter(f->workspace,UMI_TRADING_WORKSPACE_ORDERS_OPEN)==UMI_STATUS_OK);
        } else if(strcmp(name,"orders")==0) {
            UmiRiskDecision decision;
            CHECK(umi_trading_workspace_set_draft_quantity(f->workspace,1.0)==UMI_STATUS_OK);
            CHECK(umi_trading_workspace_set_draft_prices(f->workspace,3396.0,0.0)==UMI_STATUS_OK);
            CHECK(umi_trading_workspace_submit_order(f->workspace,1000000,&decision)==UMI_STATUS_OK);
            CHECK(decision.allowed);
        } else if(strcmp(name,"preview")==0) {
            UmiRiskDecision decision;
            (void)UmiTradingUiControllerPreviewOrderAt(&f->controller,1000000,&decision);
        } else if(strcmp(name,"defaults")!=0 && strcmp(name,"empty")!=0 && strcmp(name,"notify")!=0) return 2;
        CHECK(umi_trading_workspace_snapshot(f->workspace,&before)==UMI_STATUS_OK);
        unsigned count=f->notifications;
        CHECK(umi_trading_ui_controller_dispatch(&f->controller,UMI_TRADING_UI_ACTION_RESET_DRAFT,NULL,NULL)==UMI_STATUS_OK);
        CHECK(f->notifications==count+1);
        CHECK(umi_trading_workspace_snapshot(f->workspace,&after)==UMI_STATUS_OK);
        CHECK(after.draft_order.side==UMI_SIDE_BUY && after.draft_order.type==UMI_ORDER_LIMIT);
        CHECK(after.draft_order.tif==UMI_TIF_DAY && after.draft_order.quantity==1.0 && after.draft_order.stop_price==0.0);
        CHECK(!after.has_draft_risk);
        CHECK(after.environment==before.environment && after.live_armed==before.live_armed &&
              after.kill_switch_engaged==before.kill_switch_engaged);
        CHECK(strcmp(after.kill_switch_reason,before.kill_switch_reason)==0);
        CHECK(strcmp(after.account_id.value,before.account_id.value)==0);
        CHECK(strcmp(after.selected_instrument_id,before.selected_instrument_id)==0);
        CHECK(strcmp(after.selected_order_id,before.selected_order_id)==0);
        CHECK(strcmp(after.instrument_filter,before.instrument_filter)==0 && after.order_filter==before.order_filter);
        CHECK(after.order_count==before.order_count && after.execution_count==before.execution_count &&
              after.position_count==before.position_count);
        CHECK(after.revision==before.revision+1);
        if(strcmp(name,"orders")==0)CHECK(after.order_count==1);
        if(strcmp(name,"safeguards")==0)CHECK(!after.can_submit_order);
        if(strcmp(name,"empty")==0)CHECK(!after.has_selected_instrument && after.draft_order.limit_price==0.0);
        else CHECK(after.draft_order.limit_price==after.selected_mid);
        UmiTradingUiControllerSnapshot controller=umi_trading_ui_controller_snapshot(&f->controller);
        CHECK(!controller.last_risk.allowed);
    }
    return 0;
}
int main(int argc,char **argv) {
    if(argc!=2)return 2;
    Fixture f={0};int result=Setup(&f,strcmp(argv[1],"empty")!=0);
    if(!result)result=Run(&f,argv[1]);
    umi_trading_ui_controller_set_changed_handler(&f.controller,NULL,NULL);
    umi_trading_workspace_destroy(f.workspace);return result;
}
