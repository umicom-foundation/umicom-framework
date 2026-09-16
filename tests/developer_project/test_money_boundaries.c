/*-----------------------------------------------------------------------------
 * Umicom Framework / Studio qualification
 * File: framework/tests/developer_project/test_money_boundaries.c
 * PURPOSE: Verify checked shared money arithmetic including aliasing and malformed currencies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)

#include <stdint.h>
#include "umicom/finance/money.h"
int main(void)
{
    UmiMoney left = {10000, 2U, {{'G','B','P','\0'}}};
    UmiMoney right = {2500, 2U, {{'G','B','P','\0'}}};
    UmiMoney result = {77, 2U, {{'G','B','P','\0'}}};
    CHECK(umi_money_add(&left, &right, &result) == UMI_STATUS_OK && result.minor_units == 12500);
    CHECK(umi_money_subtract(&left, &right, &right) == UMI_STATUS_OK && right.minor_units == 7500);
    left.minor_units = INT64_MAX; right.minor_units = 1;
    CHECK(umi_money_add(&left, &right, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result.minor_units == 12500);
    left.minor_units = INT64_MIN; right.minor_units = -1;
    CHECK(umi_money_add(&left, &right, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    right.minor_units = 1;
    CHECK(umi_money_subtract(&left, &right, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    left.minor_units = 0; right.minor_units = INT64_MIN;
    CHECK(umi_money_subtract(&left, &right, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    left.minor_units = INT64_MAX; right.minor_units = -1;
    CHECK(umi_money_subtract(&left, &right, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    left.minor_units = INT64_MIN;
    CHECK(umi_money_subtract(&left, &left, &result) == UMI_STATUS_OK && result.minor_units == 0);
    memset(left.currency.code, 'X', sizeof(left.currency.code));
    CHECK(umi_money_add(&left, &right, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_money_subtract(NULL, &right, &result) == UMI_STATUS_INVALID_ARGUMENT);
    puts("Money boundaries passed.");
    return EXIT_SUCCESS;
}
