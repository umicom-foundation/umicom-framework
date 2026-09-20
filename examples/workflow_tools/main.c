/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/workflow_tools/main.c
 * PURPOSE:
 *   Read exact financial amounts, review funds and inspect an application filter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/decimal.h"
#include "umicom/finance_ui/funds_review.h"
#include "umicom/desktop/application_filter.h"
#include <stdio.h>
#include <string.h>

/* A scale of 2 stores GBP 125.50 as the integer 12550. No double conversion
 * takes place. This helper only chooses the currency for this exercise. */
static UmiStatus ReadGbp(const char *text, UmiMoney *out)
{
    UmiDecimal decimal;
    UmiStatus status = UmiDecimalParse(text, strlen(text), 2U, &decimal);
    if (status != UMI_STATUS_OK) return status;
    UmiMoney money = {0};
    money.minor_units = decimal.coefficient;
    money.scale = decimal.scale;
    memcpy(money.currency.code, "GBP", 4U);
    *out = money;
    return UMI_STATUS_OK;
}

static int ShowMoney(const char *caption, UmiMoney money)
{
    char text[UMI_DECIMAL_TEXT_MAX + 1U];
    UmiStatus status = UmiDecimalFormat(
        (UmiDecimal){money.minor_units, money.scale}, text, sizeof text);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "%s: %s\n", caption, umi_status_text(status));
        return 1;
    }
    printf("%s: %s %s\n", caption, money.currency.code, text);
    return 0;
}

int main(void)
{
    UmiMoney ledger, reserved, proposed, inflows, outflows, buffer;
    if (ReadGbp("1000.00", &ledger) != UMI_STATUS_OK ||
        ReadGbp("100.00", &reserved) != UMI_STATUS_OK ||
        ReadGbp("250.00", &proposed) != UMI_STATUS_OK ||
        ReadGbp("400.00", &inflows) != UMI_STATUS_OK ||
        ReadGbp("550.00", &outflows) != UMI_STATUS_OK ||
        ReadGbp("200.00", &buffer) != UMI_STATUS_OK) {
        fputs("An example input could not be represented.\n", stderr);
        return 1;
    }

    /* These are manual reviews, not requests to move money. The structures
     * are caller-owned stack values. The functions retain no pointers. */
    UmiFinanceAccountReview account;
    UmiStatus status = UmiFinanceReviewAccount(&ledger, &reserved, &proposed, &account);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Account review: %s\n", umi_status_text(status));
        return 1;
    }
    if (ShowMoney("Available", account.available) ||
        ShowMoney("Remaining after proposed debit", account.remaining_after_payment)) return 1;

    UmiFinanceCashReview cash;
    status = UmiFinanceReviewCash(&ledger, &inflows, &outflows, &buffer, &cash);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cash forecast: %s\n", umi_status_text(status));
        return 1;
    }
    if (ShowMoney("Net flow", cash.net_flow) || ShowMoney("Projected cash", cash.projected) ||
        ShowMoney("Buffer headroom", cash.buffer_headroom)) return 1;
    /* Independently check the expected minor-unit results, not just OK. */
    if (account.available.minor_units != 90000 ||
        account.remaining_after_payment.minor_units != 65000 ||
        cash.net_flow.minor_units != -15000 || cash.projected.minor_units != 85000 ||
        cash.buffer_headroom.minor_units != 65000 || ledger.minor_units != 100000) return 1;

    /* The filter consumes a presentation snapshot. It does not launch or
     * stop anything. Here the snapshot describes a selected, stopped app. */
    UmiApplicationLaunchChoice choice = {0};
    choice.selected = true;
    choice.state = UMI_APPLICATION_RUNTIME_STOPPED;
    bool visible = false;
    if (UmiDeskApplicationFilterMatches(&choice, UMI_DESK_APPLICATION_FILTER_SELECTED,
        &visible) != UMI_STATUS_OK || !visible) return 1;
    if (UmiDeskApplicationFilterMatches(&choice, UMI_DESK_APPLICATION_FILTER_RUNNING,
        &visible) != UMI_STATUS_OK || visible) return 1;
    puts("Selected app is visible under Selected, hidden under Running.");
    puts("No account, payment, trade, file or application process was changed.");
    return 0;
}
