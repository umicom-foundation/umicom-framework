/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance_ui/finance_ui.h
 *
 * PURPOSE:
 *   Project existing banking and treasury records into toolkit-neutral UI view
 *   models shared by Bank, TMS, Accountant and other financial applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_UI_FINANCE_UI_H
#define UMICOM_FINANCE_UI_FINANCE_UI_H

#include "umicom/finance/banking/banking_snapshot.h"
#include "umicom/finance/treasury/cash_forecast.h"
#include "umicom/finance/treasury/cash_position.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCE_UI_VIEW_BANKING_SUMMARY "umicom.finance.banking.summary"
#define UMI_FINANCE_UI_VIEW_CASH_POSITION "umicom.finance.treasury.cash-position"
#define UMI_FINANCE_UI_VIEW_CASH_FORECAST "umicom.finance.treasury.cash-forecast"

/* Build a read-only banking summary view from an existing Framework snapshot.
 * Bank and operations clients can render the returned generic view model. */
UmiStatus umi_finance_ui_banking_summary_view_create(
    const char *view_id,
    const UmiBankingBankingSnapshot *snapshot,
    UmiUiViewModel **out_view);
/* Build a read-only cash-position view from an existing treasury position.
 * TMS, Bank and Accountant can share this projection without copying calculations. */
UmiStatus umi_finance_ui_cash_position_view_create(
    const char *view_id,
    const UmiTreasuryCashPosition *position,
    UmiUiViewModel **out_view);
/* Build a read-only cash-forecast view using the existing treasury net calculation. */
UmiStatus umi_finance_ui_cash_forecast_view_create(
    const char *view_id,
    const UmiTreasuryCashForecast *forecast,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif
#endif
