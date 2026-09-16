/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/account_totals.c
 * PURPOSE: Use shared money operations for a GBP training account, without real payments.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "umicom/finance/money.h"

/* This is training data, not an account connected to a bank. */
int main(void)
{
    UmiMoney balance = {10000, 2U, {{'G', 'B', 'P', '\0'}}};
    const int64_t deposits[] = {2500, 500, 1250};
    UmiMoney payment = {3250, 2U, {{'G', 'B', 'P', '\0'}}};

    for (size_t index = 0U; index < sizeof(deposits) / sizeof(deposits[0]); ++index) {
        UmiMoney deposit = {deposits[index], 2U, {{'G', 'B', 'P', '\0'}}};
        if (umi_money_add(&balance, &deposit, &balance) != UMI_STATUS_OK) {
            fputs("Deposit cannot be represented. Balance was not changed.\n", stderr);
            return EXIT_FAILURE;
        }
    }
    /* Application policy is checked before the shared arithmetic. */
    if (payment.minor_units > balance.minor_units) {
        fputs("Payment rejected: insufficient training balance.\n", stderr);
        return EXIT_FAILURE;
    }
    if (umi_money_subtract(&balance, &payment, &balance) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    printf("Umicom training account: GBP %" PRId64 ".%02" PRId64 "\n",
        balance.minor_units / 100, balance.minor_units % 100);
    return balance.minor_units == 11000 ? EXIT_SUCCESS : EXIT_FAILURE;
}
