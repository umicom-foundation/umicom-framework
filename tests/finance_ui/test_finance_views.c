/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_ui/test_finance_views.c
 *
 * PURPOSE:
 *   Verify shared finance UI projections reuse existing banking and treasury records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/finance_ui/finance_ui.h"

int main(void)
{
    UmiBankingBankingSnapshot banking;
    UmiTreasuryCashPosition position;
    UmiTreasuryCashForecast forecast;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(umi_banking_banking_snapshot_init(&banking, "bank", 12U, 20U, 4U,
        500000, 150000) == UMI_STATUS_OK);
    assert(umi_finance_ui_banking_summary_view_create("bank-summary", &banking, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "banking.net-funding-minor", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 350000);
    umi_ui_view_model_destroy(view);

    view = NULL;
    assert(umi_treasury_cash_position_init(&position, "cash", "GBP", 100000, 125000) == UMI_STATUS_OK);
    assert(umi_finance_ui_cash_position_view_create("cash-position", &position, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "treasury.delta-minor", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 25000);
    umi_ui_view_model_destroy(view);

    view = NULL;
    assert(umi_treasury_cash_forecast_init(&forecast, "forecast", 1000, 70000, 50000) == UMI_STATUS_OK);
    assert(umi_finance_ui_cash_forecast_view_create("cash-forecast", &forecast, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "treasury.net-minor", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 20000);
    umi_ui_view_model_destroy(view);
    return 0;
}
