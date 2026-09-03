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

/**
 * Represent the trading ui controller config data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingUiControllerConfig {
    int allow_live_environment;
} UmiTradingUiControllerConfig;

/**
 * Represent the trading ui action payload data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingUiActionPayload {
    const char *text;
    double primary_number;
    double secondary_number;
    int64_t event_time_ms;
    int primary_value;
    int secondary_value;
} UmiTradingUiActionPayload;

/**
 * Represent the trading ui controller snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingUiControllerSnapshot {
    uint64_t revision;
    UmiStatus last_status;
    UmiRiskDecision last_risk;
    char last_message[UMI_TRADING_UI_CONTROLLER_MESSAGE_CAPACITY];
    int live_environment_allowed;
} UmiTradingUiControllerSnapshot;

/**
 * Represent the trading ui controller data shared with callers of this public contract.
 */
typedef struct UmiTradingUiController {
    UmiTradingWorkspace *workspace;
    UmiTradingUiControllerConfig config;
    UmiTradingUiChangedHandler changed_handler;
    void *changed_user_data;
    UmiTradingUiControllerSnapshot state;
    uint64_t next_alert_sequence;
} UmiTradingUiController;

/**
 * Provide the trading ui controller config default operation used by this module and its
 * client applications.
 */
UmiTradingUiControllerConfig umi_trading_ui_controller_config_default(void);
/**
 * Initialise trading ui controller from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_trading_ui_controller_init(
    UmiTradingUiController *controller,
    UmiTradingWorkspace *workspace,
    const UmiTradingUiControllerConfig *config);
/**
 * Provide the trading ui controller set changed handler operation used by this module and
 * its client applications.
 */
void umi_trading_ui_controller_set_changed_handler(
    UmiTradingUiController *controller,
    UmiTradingUiChangedHandler handler,
    void *user_data);

/**
 * Provide the trading ui controller refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_ui_controller_refresh(UmiTradingUiController *controller);
/**
 * Provide the trading ui controller set environment operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_set_environment(
    UmiTradingUiController *controller,
    UmiTradingEnvironment environment);
/**
 * Provide the trading ui controller set instrument filter operation used by this module
 * and its client applications.
 */
UmiStatus umi_trading_ui_controller_set_instrument_filter(
    UmiTradingUiController *controller,
    const char *filter_text);
/**
 * Provide the trading ui controller select instrument operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_ui_controller_select_instrument(
    UmiTradingUiController *controller,
    const char *instrument_id);
/**
 * Provide the trading ui controller set order filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_set_order_filter(
    UmiTradingUiController *controller,
    UmiTradingWorkspaceOrderFilter filter);
/**
 * Provide the trading ui controller select order operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_select_order(
    UmiTradingUiController *controller,
    const char *client_order_id);
/**
 * Provide the trading ui controller set draft side operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_set_draft_side(
    UmiTradingUiController *controller,
    UmiSide side);
/**
 * Provide the trading ui controller set draft type operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_set_draft_type(
    UmiTradingUiController *controller,
    UmiOrderType type,
    UmiTimeInForce tif);
/**
 * Provide the trading ui controller set draft quantity operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_ui_controller_set_draft_quantity(
    UmiTradingUiController *controller,
    double quantity);
/**
 * Provide the trading ui controller set draft prices operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_set_draft_prices(
    UmiTradingUiController *controller,
    double limit_price,
    double stop_price);
/**
 * Provide the trading ui controller preview order operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_preview_order(
    UmiTradingUiController *controller,
    UmiRiskDecision *out_decision);
/**
 * Provide the trading ui controller submit order operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_ui_controller_submit_order(
    UmiTradingUiController *controller,
    int64_t now_ms,
    UmiRiskDecision *out_decision);
/**
 * Provide the trading ui controller cancel selected order operation used by this module
 * and its client applications.
 */
UmiStatus umi_trading_ui_controller_cancel_selected_order(
    UmiTradingUiController *controller);
/**
 * Provide the trading ui controller engage kill switch operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_ui_controller_engage_kill_switch(
    UmiTradingUiController *controller,
    const char *reason);
/**
 * Provide the trading ui controller reset kill switch operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_ui_controller_reset_kill_switch(
    UmiTradingUiController *controller);

/** Create a price alert for the currently selected instrument. */
UmiStatus umi_trading_ui_controller_create_price_alert(
    UmiTradingUiController *controller,
    UmiTradingPriceAlertDirection direction,
    double threshold,
    int64_t now_ms);

/** Pause or resume a price alert selected by stable identifier. */
UmiStatus umi_trading_ui_controller_set_price_alert_enabled(
    UmiTradingUiController *controller,
    const char *alert_id,
    int enabled);

/** Acknowledge an active price alert selected by stable identifier. */
UmiStatus umi_trading_ui_controller_acknowledge_price_alert(
    UmiTradingUiController *controller,
    const char *alert_id);

/** Remove a price alert selected by stable identifier. */
UmiStatus umi_trading_ui_controller_remove_price_alert(
    UmiTradingUiController *controller,
    const char *alert_id);

/** Apply one shared direction and minimum-size filter to Time and Sales. */
UmiStatus umi_trading_ui_controller_set_trade_tape_filter(
    UmiTradingUiController *controller,
    UmiTradingTradeTapeFilter filter,
    double minimum_size);

/** Pause or resume the visible Time and Sales sequence without stopping data. */
UmiStatus umi_trading_ui_controller_set_trade_tape_paused(
    UmiTradingUiController *controller,
    int paused);

/**
 * Perform trading ui controller through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_trading_ui_controller_dispatch(
    UmiTradingUiController *controller,
    const char *action_id,
    const UmiTradingUiActionPayload *payload,
    UmiRiskDecision *out_decision);
/**
 * Provide the trading ui controller snapshot operation used by this module and its client
 * applications.
 */
UmiTradingUiControllerSnapshot umi_trading_ui_controller_snapshot(
    const UmiTradingUiController *controller);

#ifdef __cplusplus
}
#endif
#endif
