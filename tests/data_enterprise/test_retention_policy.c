/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_retention_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the retention policy enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/retention_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataRetentionPolicy policy;
    bool allowed = false;
    umi_data_retention_policy_default(&policy);
    policy.max_age_ms=1000U; policy.minimum_copies=2U; policy.preserve_last_success=true;
    CHECK(umi_data_retention_policy_evaluate(&policy,2000U,4U,false,&allowed) == UMI_STATUS_OK);
    CHECK(allowed == true);
    return 0;
}
