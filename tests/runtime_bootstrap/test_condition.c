/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_condition.c
 *
 * PURPOSE:
 *   Implement the test condition behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_condition.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe one property, feature, capability, platform or environment condition.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/condition.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapCondition c;
    CHECK(umi_bootstrap_condition_init(&c,UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS,"app.mode","prod",false)==UMI_STATUS_OK);
    CHECK(strcmp(c.key,"app.mode")==0);
    return 0;
}
