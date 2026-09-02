/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_lifecycle_policy.c
 *
 * PURPOSE:
 *   Exercise the lifecycle policy financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/lifecycle_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLifecyclePolicy p; umi_lifecycle_policy_default(&p); CHECK(umi_lifecycle_policy_allows(&p,10U,1U));
    return 0;
}
