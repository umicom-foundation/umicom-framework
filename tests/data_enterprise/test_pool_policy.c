/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_pool_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the pool policy enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/pool_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataPoolPolicy policy;
    bool allowed = false;
    umi_data_pool_policy_default(&policy);
    policy.minimum_size=1U; policy.maximum_size=8U; policy.high_utilisation_percent=75.0;
    CHECK(umi_data_pool_policy_evaluate(&policy,4U,90.0,&allowed) == UMI_STATUS_OK);
    CHECK(allowed == true);
    return 0;
}
