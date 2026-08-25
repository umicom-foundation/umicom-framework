/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_failover_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the failover policy enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/failover_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataFailoverPolicy policy;
    bool allowed = false;
    umi_data_failover_policy_default(&policy);
    policy.maximum_promotion_lag_ms=100U; policy.automatic_failover=true; policy.require_manual_approval=true;
    CHECK(umi_data_failover_policy_evaluate(&policy,true,50U,true,&allowed) == UMI_STATUS_OK);
    CHECK(allowed == true);
    return 0;
}
