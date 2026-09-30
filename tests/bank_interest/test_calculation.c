/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_interest/test_calculation.c
 * PURPOSE: Check exact rounding, large factors and rejected arithmetic inputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/finance/banking/interest_accrual.h"
#include <limits.h>
int main(int argc, char **argv)
{
    UmiBankingInterestAccrual value; int64_t amount = 123;
    CHECK(argc == 2); OK(umi_banking_interest_accrual_init(&value,"calculation",100000,500,30,365));
    if (strcmp(argv[1],"rounding") == 0) {
        OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == 410);
        value.annual_rate_bps = -500; OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == -410);
        value.day_count_basis = 360; OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == -416);
        value.days = 0; OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == 0);
    } else if (strcmp(argv[1],"large") == 0) {
        value.principal_minor = INT64_MAX; value.annual_rate_bps = 1; value.days = 1;
        OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == INT64_MAX / 3650000);
        value.annual_rate_bps = 10000; value.days = 365;
        OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == INT64_MAX);
        value.annual_rate_bps = -10000; OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == -INT64_MAX);
        value.principal_minor = INT64_C(4611686018427387904); value.annual_rate_bps = -20000;
        OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == INT64_MIN);
        value.principal_minor++;
        CHECK(umi_banking_interest_accrual_calculate(&value,&amount) == UMI_STATUS_CAPACITY_EXCEEDED && amount == INT64_MIN);
        value.principal_minor = 1; value.annual_rate_bps = INT32_MIN; value.days = 3660;
        OK(umi_banking_interest_accrual_calculate(&value,&amount)); CHECK(amount == -2153367);
    } else if (strcmp(argv[1],"overflow") == 0) {
        value.principal_minor = INT64_MAX; value.annual_rate_bps = 10001; value.days = 365;
        CHECK(umi_banking_interest_accrual_calculate(&value,&amount) == UMI_STATUS_CAPACITY_EXCEEDED && amount == 123);
        CHECK(umi_banking_interest_accrual_accrued_minor(&value) == 0);
    } else if (strcmp(argv[1],"invalid") == 0) {
        CHECK(umi_banking_interest_accrual_calculate(NULL,&amount) == UMI_STATUS_INVALID_ARGUMENT && amount == 123);
        CHECK(umi_banking_interest_accrual_calculate(&value,NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_banking_interest_accrual_calculate(&value,&value.principal_minor) == UMI_STATUS_INVALID_ARGUMENT && value.principal_minor == 100000);
        value.day_count_basis = 0; CHECK(umi_banking_interest_accrual_calculate(&value,&amount) == UMI_STATUS_INVALID_ARGUMENT && amount == 123);
        CHECK(umi_banking_interest_accrual_accrued_minor(&value) == 0);
        value.day_count_basis = 365; value.days = 3661;
        CHECK(umi_banking_interest_accrual_calculate(&value,&amount) == UMI_STATUS_INVALID_ARGUMENT);
        value.days = 1; value.principal_minor = -1;
        CHECK(umi_banking_interest_accrual_calculate(&value,&amount) == UMI_STATUS_INVALID_ARGUMENT);
    } else CHECK(0);
    return 0;
}
