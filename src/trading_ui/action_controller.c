/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/action_controller.c
 *
 * PURPOSE:
 *   Route trading UI mutations through the canonical trading workspace and its
 *   existing simulation, broker-readiness, live-arming and risk safeguards.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_ui/action_controller.h"

#include <stdio.h>
#include <string.h>

static void copy_message(char *destination, size_t capacity, const char *text)
{
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", text != NULL ? text : "");
}

static const char *status_message(UmiStatus status)
{
    switch (status) {
    case UMI_STATUS_OK: return "Trading workspace updated.";
    case UMI_STATUS_PERMISSION_DENIED: return "Trading action denied by policy.";
    case UMI_STATUS_INVALID_STATE: return "Trading workspace is not ready for this action.";
    case UMI_STATUS_NOT_FOUND: return "Requested trading item was not found.";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "Trading workspace capacity was exceeded.";
    default: return "Trading action did not complete.";
    }
}

static UmiStatus finish_action(UmiTradingUiController *controller,
                               UmiStatus status,
                               const UmiRiskDecision *decision,
                               const char *message,
                               int notify)
{
    if (controller == NULL) return status;
    controller->state.last_status = status;
    if (decision != NULL) controller->state.last_risk = *decision;
    copy_message(controller->state.last_message,
                 sizeof(controller->state.last_message),
                 message != NULL ? message : status_message(status));
    if (status == UMI_STATUS_OK) {
        controller->state.revision += 1U;
        if (notify && controller->changed_handler != NULL)
            controller->changed_handler(controller->state.revision,
                                        controller->changed_user_data);
    }
    return status;
}

static int valid_environment(UmiTradingEnvironment environment)
{
    return environment >= UMI_TRADING_SIMULATION &&
           environment <= UMI_TRADING_LIVE;
}

UmiTradingUiControllerConfig umi_trading_ui_controller_config_default(void)
{
    UmiTradingUiControllerConfig config;
    config.allow_live_environment = 0;
    return config;
}

UmiStatus umi_trading_ui_controller_init(
    UmiTradingUiController *controller,
    UmiTradingWorkspace *workspace,
    const UmiTradingUiControllerConfig *config)
{
    UmiTradingUiControllerConfig effective;
    if (controller == NULL || workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    effective = config != NULL
        ? *config : umi_trading_ui_controller_config_default();
    (void)memset(controller, 0, sizeof(*controller));
    controller->workspace = workspace;
    controller->config = effective;
    controller->state.revision = 1U;
    controller->state.last_status = UMI_STATUS_OK;
    controller->state.live_environment_allowed =
        effective.allow_live_environment != 0;
    copy_message(controller->state.last_message,
                 sizeof(controller->state.last_message),
                 "Trading UI controller ready.");
    return UMI_STATUS_OK;
}

void umi_trading_ui_controller_set_changed_handler(
    UmiTradingUiController *controller,
    UmiTradingUiChangedHandler handler,
    void *user_data)
{
    if (controller == NULL) return;
    controller->changed_handler = handler;
    controller->changed_user_data = user_data;
}

UmiStatus umi_trading_ui_controller_refresh(UmiTradingUiController *controller)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_refresh(controller->workspace);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_set_environment(
    UmiTradingUiController *controller,
    UmiTradingEnvironment environment)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL ||
        !valid_environment(environment))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (environment == UMI_TRADING_LIVE &&
        !controller->config.allow_live_environment) {
        return finish_action(controller, UMI_STATUS_PERMISSION_DENIED, NULL,
            "Live environment selection is disabled by workstation policy.", 0);
    }
    status = umi_trading_workspace_set_environment(
        controller->workspace, environment);
    return finish_action(controller, status, NULL,
        status == UMI_STATUS_OK
            ? "Trading environment changed. Live order execution remains separately armed."
            : NULL,
        1);
}

UmiStatus umi_trading_ui_controller_set_instrument_filter(
    UmiTradingUiController *controller,
    const char *filter_text)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL || filter_text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_instrument_filter(
        controller->workspace, filter_text);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_select_instrument(
    UmiTradingUiController *controller,
    const char *instrument_id)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL ||
        instrument_id == NULL || instrument_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_select_instrument(
        controller->workspace, instrument_id);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_set_order_filter(
    UmiTradingUiController *controller,
    UmiTradingWorkspaceOrderFilter filter)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_order_filter(controller->workspace, filter);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_select_order(
    UmiTradingUiController *controller,
    const char *client_order_id)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL ||
        client_order_id == NULL || client_order_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_select_order(
        controller->workspace, client_order_id);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_set_draft_side(
    UmiTradingUiController *controller,
    UmiSide side)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_draft_side(controller->workspace, side);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_set_draft_type(
    UmiTradingUiController *controller,
    UmiOrderType type,
    UmiTimeInForce tif)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_draft_type(controller->workspace, type, tif);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_set_draft_quantity(
    UmiTradingUiController *controller,
    double quantity)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_draft_quantity(
        controller->workspace, quantity);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_set_draft_prices(
    UmiTradingUiController *controller,
    double limit_price,
    double stop_price)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_draft_prices(
        controller->workspace, limit_price, stop_price);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_preview_order(
    UmiTradingUiController *controller,
    UmiRiskDecision *out_decision)
{
    UmiRiskDecision decision = {0};
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_preview_order(controller->workspace, &decision);
    if (out_decision != NULL) *out_decision = decision;
    return finish_action(controller, status, &decision,
        status == UMI_STATUS_OK
            ? (decision.allowed ? "Order preview passed risk controls."
                                : "Order preview was rejected by risk controls.")
            : NULL,
        1);
}

UmiStatus umi_trading_ui_controller_submit_order(
    UmiTradingUiController *controller,
    int64_t now_ms,
    UmiRiskDecision *out_decision)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiRiskDecision decision = {0};
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL || now_ms < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_snapshot(controller->workspace, &snapshot);
    if (status != UMI_STATUS_OK)
        return finish_action(controller, status, NULL, NULL, 0);
    if (!snapshot.has_draft_risk || !snapshot.can_submit_order) {
        return finish_action(controller, UMI_STATUS_INVALID_STATE, NULL,
            "Preview and pass risk controls before submitting the order.", 0);
    }
    status = umi_trading_workspace_submit_order(
        controller->workspace, now_ms, &decision);
    if (out_decision != NULL) *out_decision = decision;
    return finish_action(controller, status, &decision,
        status == UMI_STATUS_OK ? "Order submitted through the guarded workspace." : NULL,
        1);
}

UmiStatus umi_trading_ui_controller_cancel_selected_order(
    UmiTradingUiController *controller)
{
    UmiStatus status;
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_cancel_selected_order(controller->workspace);
    return finish_action(controller, status, NULL, NULL, 1);
}

UmiStatus umi_trading_ui_controller_engage_kill_switch(
    UmiTradingUiController *controller,
    const char *reason)
{
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    umi_trading_workspace_engage_kill_switch(
        controller->workspace,
        reason != NULL && reason[0] != '\0'
            ? reason : "Requested from Framework trading workstation");
    return finish_action(controller, UMI_STATUS_OK, NULL,
        "Trading kill switch engaged.", 1);
}

UmiStatus umi_trading_ui_controller_reset_kill_switch(
    UmiTradingUiController *controller)
{
    if (controller == NULL || controller->workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    umi_trading_workspace_reset_kill_switch(controller->workspace);
    return finish_action(controller, UMI_STATUS_OK, NULL,
        "Trading kill switch reset through the canonical workspace.", 1);
}

UmiStatus umi_trading_ui_controller_dispatch(
    UmiTradingUiController *controller,
    const char *action_id,
    const UmiTradingUiActionPayload *payload,
    UmiRiskDecision *out_decision)
{
    UmiTradingUiActionKind kind = umi_trading_ui_action_kind(action_id);
    if (controller == NULL || action_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_trading_ui_action_requires_payload(kind) && payload == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    switch (kind) {
    case UMI_TRADING_UI_ACTION_KIND_REFRESH:
        return umi_trading_ui_controller_refresh(controller);
    case UMI_TRADING_UI_ACTION_KIND_SET_ENVIRONMENT:
        return umi_trading_ui_controller_set_environment(
            controller, (UmiTradingEnvironment)payload->primary_value);
    case UMI_TRADING_UI_ACTION_KIND_FILTER_INSTRUMENTS:
        return umi_trading_ui_controller_set_instrument_filter(
            controller, payload->text != NULL ? payload->text : "");
    case UMI_TRADING_UI_ACTION_KIND_SELECT_INSTRUMENT:
        return umi_trading_ui_controller_select_instrument(controller, payload->text);
    case UMI_TRADING_UI_ACTION_KIND_SET_SIDE:
        return umi_trading_ui_controller_set_draft_side(
            controller, (UmiSide)payload->primary_value);
    case UMI_TRADING_UI_ACTION_KIND_SET_ORDER_TYPE:
        return umi_trading_ui_controller_set_draft_type(
            controller,
            (UmiOrderType)payload->primary_value,
            (UmiTimeInForce)payload->secondary_value);
    case UMI_TRADING_UI_ACTION_KIND_SET_QUANTITY:
        return umi_trading_ui_controller_set_draft_quantity(
            controller, payload->primary_number);
    case UMI_TRADING_UI_ACTION_KIND_SET_PRICES:
        return umi_trading_ui_controller_set_draft_prices(
            controller, payload->primary_number, payload->secondary_number);
    case UMI_TRADING_UI_ACTION_KIND_PREVIEW_ORDER:
        return umi_trading_ui_controller_preview_order(controller, out_decision);
    case UMI_TRADING_UI_ACTION_KIND_SUBMIT_ORDER:
        return umi_trading_ui_controller_submit_order(
            controller, payload != NULL ? payload->event_time_ms : 0, out_decision);
    case UMI_TRADING_UI_ACTION_KIND_FILTER_ORDERS:
        return umi_trading_ui_controller_set_order_filter(
            controller, (UmiTradingWorkspaceOrderFilter)payload->primary_value);
    case UMI_TRADING_UI_ACTION_KIND_SELECT_ORDER:
        return umi_trading_ui_controller_select_order(controller, payload->text);
    case UMI_TRADING_UI_ACTION_KIND_CANCEL_ORDER:
        return umi_trading_ui_controller_cancel_selected_order(controller);
    case UMI_TRADING_UI_ACTION_KIND_ENGAGE_KILL_SWITCH:
        return umi_trading_ui_controller_engage_kill_switch(
            controller, payload != NULL ? payload->text : NULL);
    case UMI_TRADING_UI_ACTION_KIND_RESET_KILL_SWITCH:
        return umi_trading_ui_controller_reset_kill_switch(controller);
    default:
        return finish_action(controller, UMI_STATUS_NOT_FOUND, NULL,
            "Unknown trading workstation action.", 0);
    }
}

UmiTradingUiControllerSnapshot umi_trading_ui_controller_snapshot(
    const UmiTradingUiController *controller)
{
    return controller != NULL
        ? controller->state : (UmiTradingUiControllerSnapshot){0};
}
