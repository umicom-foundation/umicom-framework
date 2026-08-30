/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_consistency_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the consistency policy enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/consistency_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataConsistencyPolicy policy;
    bool allowed = false;
    umi_data_consistency_policy_default(&policy);
    policy.session_lag_ms=100U; policy.eventual_lag_ms=1000U;
    CHECK(umi_data_consistency_policy_evaluate(&policy,UMI_DATA_CONSISTENCY_SESSION,50U,false,&allowed) == UMI_STATUS_OK);
    CHECK(allowed == true);
    return 0;
}
