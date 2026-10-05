/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/cash_planning/main.c
 * PURPOSE: Demonstrate a complete dated cash projection using the portable Framework owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cash_planning/plan.h"
#include "umicom/finance/decimal.h"
#include <stdio.h>
int main(void)
{
    /* All amounts have two decimal places: 100000 minor units means 1000.00.
     * A future host can replace these explicit inputs with reviewed user data. */
    UmiCashPlanConfig config = {.title = "Monthly cash",
                                .start = {2028, 2, 1},
                                .end = {2028, 2, 29},
                                .opening = {100000, 2, {"GBP"}},
                                .buffer_minor = 20000};
    UmiCashPlanEntry rent = {.id = "rent",
                             .label = "Office rent",
                             .date = {2028, 2, 3},
                             .amount = {85000, 2, {"GBP"}},
                             .direction = UMI_FINANCIAL_DIRECTION_PAY,
                             .enabled = 1};
    UmiCashPlanEntry receipt = {.id = "receipt",
                                .label = "Expected customer payment",
                                .date = {2028, 2, 20},
                                .amount = {50000, 2, {"GBP"}},
                                .direction = UMI_FINANCIAL_DIRECTION_RECEIVE,
                                .enabled = 1};
    UmiCashPlan *plan = NULL;
    UmiStatus status = UmiCashPlanCreate(&config, &plan);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanAdd(plan, &rent);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanAdd(plan, &receipt);
    UmiCashPlanForecast forecast;
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanProject(plan, &forecast);
    if (status == UMI_STATUS_OK)
    {
        char closing[UMI_DECIMAL_TEXT_MAX], date[11];
        status = UmiDecimalFormat((UmiDecimal){forecast.closing_minor, config.opening.scale}, closing,
                                  sizeof closing);
        if (status == UMI_STATUS_OK)
            status = UmiCashPlanDateFormat(forecast.first_shortfall_date, date, sizeof date);
        if (status == UMI_STATUS_OK)
            (void)printf("Closing: %s GBP\nFirst buffer shortfall: %s\n", closing, date);
    }
    /* The plan owns its copied entries. Destroying it does not affect the
     * caller's input structures or create any financial transaction. */
    UmiCashPlanDestroy(plan);
    if (status != UMI_STATUS_OK)
        (void)fprintf(stderr, "Cash projection refused: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
