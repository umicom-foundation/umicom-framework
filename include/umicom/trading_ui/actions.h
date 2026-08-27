/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/actions.h
 *
 * PURPOSE:
 *   Centralise professional trading workstation action identifiers so GTK4,
 *   Qt/Wt frontends and thin products route through one Framework contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_ACTIONS_H
#define UMICOM_TRADING_UI_ACTIONS_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_UI_ACTION_REFRESH \
    "studio.action.trading.refresh"
#define UMI_TRADING_UI_ACTION_SET_ENVIRONMENT \
    "studio.action.trading.set-environment"
#define UMI_TRADING_UI_ACTION_FILTER_INSTRUMENTS \
    "studio.action.trading.filter-instruments"
#define UMI_TRADING_UI_ACTION_SELECT_INSTRUMENT \
    "studio.action.trading.select-instrument"
#define UMI_TRADING_UI_ACTION_SET_SIDE \
    "studio.action.trading.set-side"
#define UMI_TRADING_UI_ACTION_SET_ORDER_TYPE \
    "studio.action.trading.set-order-type"
#define UMI_TRADING_UI_ACTION_SET_QUANTITY \
    "studio.action.trading.set-quantity"
#define UMI_TRADING_UI_ACTION_SET_PRICES \
    "studio.action.trading.set-prices"
#define UMI_TRADING_UI_ACTION_PREVIEW_ORDER \
    "studio.action.trading.preview-order"
#define UMI_TRADING_UI_ACTION_SUBMIT_ORDER \
    "studio.action.trading.submit-order"
#define UMI_TRADING_UI_ACTION_FILTER_ORDERS \
    "studio.action.trading.filter-orders"
#define UMI_TRADING_UI_ACTION_SELECT_ORDER \
    "studio.action.trading.select-order"
#define UMI_TRADING_UI_ACTION_CANCEL_ORDER \
    "studio.action.trading.cancel-order"
#define UMI_TRADING_UI_ACTION_ENGAGE_KILL_SWITCH \
    "studio.action.trading.engage-kill-switch"
#define UMI_TRADING_UI_ACTION_RESET_KILL_SWITCH \
    "studio.action.trading.reset-kill-switch"

typedef enum UmiTradingUiActionKind {
    UMI_TRADING_UI_ACTION_KIND_UNKNOWN = 0,
    UMI_TRADING_UI_ACTION_KIND_REFRESH = 1,
    UMI_TRADING_UI_ACTION_KIND_SET_ENVIRONMENT = 2,
    UMI_TRADING_UI_ACTION_KIND_FILTER_INSTRUMENTS = 3,
    UMI_TRADING_UI_ACTION_KIND_SELECT_INSTRUMENT = 4,
    UMI_TRADING_UI_ACTION_KIND_SET_SIDE = 5,
    UMI_TRADING_UI_ACTION_KIND_SET_ORDER_TYPE = 6,
    UMI_TRADING_UI_ACTION_KIND_SET_QUANTITY = 7,
    UMI_TRADING_UI_ACTION_KIND_SET_PRICES = 8,
    UMI_TRADING_UI_ACTION_KIND_PREVIEW_ORDER = 9,
    UMI_TRADING_UI_ACTION_KIND_SUBMIT_ORDER = 10,
    UMI_TRADING_UI_ACTION_KIND_FILTER_ORDERS = 11,
    UMI_TRADING_UI_ACTION_KIND_SELECT_ORDER = 12,
    UMI_TRADING_UI_ACTION_KIND_CANCEL_ORDER = 13,
    UMI_TRADING_UI_ACTION_KIND_ENGAGE_KILL_SWITCH = 14,
    UMI_TRADING_UI_ACTION_KIND_RESET_KILL_SWITCH = 15
} UmiTradingUiActionKind;

UmiTradingUiActionKind umi_trading_ui_action_kind(const char *action_id);
const char *umi_trading_ui_action_id(UmiTradingUiActionKind kind);
int umi_trading_ui_action_requires_payload(UmiTradingUiActionKind kind);

#ifdef __cplusplus
}
#endif
#endif
