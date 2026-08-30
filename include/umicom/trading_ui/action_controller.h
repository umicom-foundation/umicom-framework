/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/action_controller.h
 *
 * PURPOSE:
 *   Own safe, toolkit-neutral mutation of the professional trading workspace
 *   from user-interface actions. Thin products never bypass workspace policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_ACTION_CONTROLLER_H
#define UMICOM_TRADING_UI_ACTION_CONTROLLER_H

#include <stdint.h>

#include "umicom/trading/workspace.h"
#include "umicom/trading_ui/actions.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_UI_CONTROLLER_MESSAGE_CAPACITY 192U

typedef void (*UmiTradingUiChangedHandler)(
    uint64_t revision,
    void *user_data);

typedef struct UmiTradingUiControllerConfig {
    int allow_live_environment;
} UmiTradingUiControllerConfig;

typedef struct UmiTradingUiActionPayload {
    const char *text;
    double primary_number;
    double secondary_number;
    int64_t event_time_ms;
    int primary_value;
    int secondary_value;
} UmiTradingUiActionPayload;

typedef struct UmiTradingUiControllerSnapshot {
    uint64_t revision;
    UmiStatus last_status;
    UmiRiskDecision last_risk;
    char last_message[UMI_TRADING_UI_CONTROLLER_MESSAGE_CAPACITY];
    int live_environment_allowed;
} UmiTradingUiControllerSnapshot;

typedef struct UmiTradingUiController {
    UmiTradingWorkspace *workspace;
    UmiTradingUiControllerConfig config;
    UmiTradingUiChangedHandler changed_handler;
    void *changed_user_data;
    UmiTradingUiControllerSnapshot state;
} UmiTradingUiController;

UmiTradingUiControllerConfig umi_trading_ui_controller_config_default(void);
UmiStatus umi_trading_ui_controller_init(
    UmiTradingUiController *controller,
    UmiTradingWorkspace *workspace,
    const UmiTradingUiControllerConfig *config);
void umi_trading_ui_controller_set_changed_handler(
    UmiTradingUiController *controller,
    UmiTradingUiChangedHandler handler,
    void *user_data);

UmiStatus umi_trading_ui_controller_refresh(UmiTradingUiController *controller);
UmiStatus umi_trading_ui_controller_set_environment(
    UmiTradingUiController *controller,
    UmiTradingEnvironment environment);
UmiStatus umi_trading_ui_controller_set_instrument_filter(
    UmiTradingUiController *controller,
    const char *filter_text);
UmiStatus umi_trading_ui_controller_select_instrument(
    UmiTradingUiController *controller,
    const char *instrument_id);
UmiStatus umi_trading_ui_controller_set_order_filter(
    UmiTradingUiController *controller,
    UmiTradingWorkspaceOrderFilter filter);
UmiStatus umi_trading_ui_controller_select_order(
    UmiTradingUiController *controller,
    const char *client_order_id);
UmiStatus umi_trading_ui_controller_set_draft_side(
    UmiTradingUiController *controller,
    UmiSide side);
UmiStatus umi_trading_ui_controller_set_draft_type(
    UmiTradingUiController *controller,
    UmiOrderType type,
    UmiTimeInForce tif);
UmiStatus umi_trading_ui_controller_set_draft_quantity(
    UmiTradingUiController *controller,
    double quantity);
UmiStatus umi_trading_ui_controller_set_draft_prices(
    UmiTradingUiController *controller,
    double limit_price,
    double stop_price);
UmiStatus umi_trading_ui_controller_preview_order(
    UmiTradingUiController *controller,
    UmiRiskDecision *out_decision);
UmiStatus umi_trading_ui_controller_submit_order(
    UmiTradingUiController *controller,
    int64_t now_ms,
    UmiRiskDecision *out_decision);
UmiStatus umi_trading_ui_controller_cancel_selected_order(
    UmiTradingUiController *controller);
UmiStatus umi_trading_ui_controller_engage_kill_switch(
    UmiTradingUiController *controller,
    const char *reason);
UmiStatus umi_trading_ui_controller_reset_kill_switch(
    UmiTradingUiController *controller);

UmiStatus umi_trading_ui_controller_dispatch(
    UmiTradingUiController *controller,
    const char *action_id,
    const UmiTradingUiActionPayload *payload,
    UmiRiskDecision *out_decision);
UmiTradingUiControllerSnapshot umi_trading_ui_controller_snapshot(
    const UmiTradingUiController *controller);

#ifdef __cplusplus
}
#endif
#endif
