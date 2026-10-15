/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/policy_batch/table.c
 * PURPOSE: Refresh synthetic account observations while preserving the last complete result on refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_platform/account/balance_consistency.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiFinancialPlatformInput observations[2];
    UmiFinancialPlatformSnapshot results[2];
    unsigned char before[sizeof(results)];

    /* Each input is an observation supplied by the application. Initializing
     * every field makes the example safe to extend with another policy later. */
    for (size_t index = 0U; index < 2U; ++index) {
        umi_financial_platform_input_init(&observations[index]);
        observations[index].trusted = 1;
        observations[index].debitTotal = 1000.0;
        observations[index].revision = 1U;
    }
    observations[0].label = "Balanced example";
    observations[0].creditTotal = 1000.0;
    observations[1].label = "Difference example";
    observations[1].creditTotal = 900.0;

    UmiStatus status = umi_financial_platform_evaluate_account_balance_consistency_batch(
        observations, 2U, results, 2U);
    if (status != UMI_STATUS_OK) {
        fputs("The example table could not be evaluated.\n", stderr);
        return 1;
    }
    for (size_t index = 0U; index < 2U; ++index)
        printf("%s: difference %.2f, ready %d\n",
               results[index].label, results[index].value, results[index].ready);
    if (results[0].ready != 1 || results[1].ready != 0) return 1;

    /* A malformed new observation must not replace either previously accepted
     * row. A real host should display the refusal and retain its prior table. */
    memcpy(before, results, sizeof(results));
    observations[1].creditTotal = NAN;
    status = umi_financial_platform_evaluate_account_balance_consistency_batch(
        observations, 2U, results, 2U);
    if (status != UMI_STATUS_INVALID_ARGUMENT ||
        memcmp(before, results, sizeof(results)) != 0) return 1;
    puts("Invalid refresh refused; the previous complete table is unchanged.");
    return 0;
}
