/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_cashflow.c
 *
 * PURPOSE:
 *   Exercise the cashflow financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "umicom/finance/core/cashflow.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "%d: %s\n", __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCashflow cashflow, retained;
    const UmiMoney amount = {100, 2U, {"USD"}};
    const UmiFinancialDate date = {2026, 9U, 1U};
    CHECK(umi_cashflow_init(&cashflow, "CF", "T", amount, UMI_FINANCIAL_DIRECTION_RECEIVE, date) == UMI_STATUS_OK);
    CHECK(umi_cashflow_is_valid(&cashflow));
    memcpy(&retained, &cashflow, sizeof retained);
    CHECK(umi_cashflow_init(&cashflow, "new-cashflow", NULL, amount, UMI_FINANCIAL_DIRECTION_RECEIVE, date) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&cashflow, &retained, sizeof cashflow) == 0);
    CHECK(umi_cashflow_init(&cashflow, "CF", "T", amount, (UmiFinancialDirection)99, date) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_cashflow_init(&cashflow, "CF", "T", amount, UMI_FINANCIAL_DIRECTION_PAY,
        (UmiFinancialDate){2026, 2U, 30U}) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_cashflow_init(&cashflow, "CF", "T", (UmiMoney){100, 10U, {"USD"}},
        UMI_FINANCIAL_DIRECTION_PAY, date) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_cashflow_init(&cashflow, "CF", "T", (UmiMoney){100, 2U, {"usd"}},
        UMI_FINANCIAL_DIRECTION_PAY, date) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_cashflow_init(&cashflow, "CF", "T", (UmiMoney){100, 2U, {{'U','S','D','A'}}},
        UMI_FINANCIAL_DIRECTION_PAY, date) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&cashflow, &retained, sizeof cashflow) == 0);
    cashflow.amount.currency.code[3] = 'A';
    CHECK(!umi_cashflow_is_valid(&cashflow));
    cashflow = retained;
    cashflow.direction = (UmiFinancialDirection)99;
    CHECK(!umi_cashflow_is_valid(&cashflow));
    CHECK(umi_cashflow_init(&cashflow, retained.cashflow_id.value, retained.trade_id.id.value,
        amount, UMI_FINANCIAL_DIRECTION_PAY, date) == UMI_STATUS_OK && umi_cashflow_is_valid(&cashflow));
    return 0;
}
