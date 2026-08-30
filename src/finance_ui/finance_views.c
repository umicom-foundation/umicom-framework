/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_ui/finance_views.c
 *
 * PURPOSE:
 *   Build reusable banking and treasury view models from existing Framework
 *   finance snapshots without introducing a second financial state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance_ui/finance_ui.h"

/* Store one integer in the existing generic view-model property bag. */
static UmiStatus set_integer(UmiUiViewModel *view, const char *key, int64_t value_number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, value_number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Store one text value without introducing a finance-specific property container. */
static UmiStatus set_string(UmiUiViewModel *view, const char *key, const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Create the canonical generic pane model used by every frontend adapter. */
static UmiStatus create_view(const char *view_id, const char *view_kind,
                             UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (view_id == NULL || view_kind == NULL || out_view == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_view_model_create(view_id, "umicom.finance-ui",
                                      UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "umicom.view-kind", view_kind);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/* Project an existing aggregate banking snapshot for Bank or operations dashboards. */
UmiStatus umi_finance_ui_banking_summary_view_create(
    const char *view_id,
    const UmiBankingBankingSnapshot *snapshot,
    UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (snapshot == NULL || !umi_banking_banking_snapshot_valid(snapshot))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "banking-summary", out_view);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "banking.customer-count", (int64_t)snapshot->customer_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "banking.deposit-account-count", (int64_t)snapshot->deposit_account_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "banking.loan-count", (int64_t)snapshot->loan_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "banking.deposit-balance-minor", snapshot->deposit_balance_minor);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "banking.loan-balance-minor", snapshot->loan_balance_minor);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "banking.net-funding-minor",
            umi_banking_banking_snapshot_net_funding_minor(snapshot));
    return status;
}

/* Project an existing treasury cash position for TMS, Bank or Accountant views. */
UmiStatus umi_finance_ui_cash_position_view_create(
    const char *view_id,
    const UmiTreasuryCashPosition *position,
    UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (position == NULL || !umi_treasury_cash_position_valid(position))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "treasury-cash-position", out_view);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "treasury.position-id", position->id);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "treasury.currency", position->currency.code);
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "treasury.settled-minor", position->settled_minor);
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "treasury.projected-minor", position->projected_minor);
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "treasury.delta-minor",
        umi_treasury_cash_position_delta_minor(position));
    return status;
}

/* Project an existing treasury forecast and its already-defined net-cash calculation. */
UmiStatus umi_finance_ui_cash_forecast_view_create(
    const char *view_id,
    const UmiTreasuryCashForecast *forecast,
    UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (forecast == NULL || !umi_treasury_cash_forecast_valid(forecast))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "treasury-cash-forecast", out_view);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "treasury.forecast-id", forecast->id);
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "treasury.horizon-end-ms", forecast->horizon_end_epoch_millis);
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "treasury.inflow-minor", forecast->inflow_minor);
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "treasury.outflow-minor", forecast->outflow_minor);
    if (status == UMI_STATUS_OK) status = set_integer(*out_view, "treasury.net-minor",
        umi_treasury_cash_forecast_net_minor(forecast));
    return status;
}
