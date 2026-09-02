/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_isolation_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the isolation policy enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/isolation_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataIsolationPolicy policy;
    bool allowed = false;
    umi_data_isolation_policy_default(&policy);
    policy.minimum_level=1U; policy.maximum_level=4U;
    CHECK(umi_data_isolation_policy_evaluate(&policy, 3U, &allowed) == UMI_STATUS_OK);
    CHECK(allowed == true);
    return 0;
}
