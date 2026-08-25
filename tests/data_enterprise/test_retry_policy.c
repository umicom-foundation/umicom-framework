/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_retry_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the retry policy enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/retry_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataRetryPolicy policy;
    bool allowed = false;
    umi_data_retry_policy_default(&policy);
    policy.max_attempts=3U;
    CHECK(umi_data_retry_policy_evaluate(&policy, 2U, true, &allowed) == UMI_STATUS_OK);
    CHECK(allowed == true);
    return 0;
}
