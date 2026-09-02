/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_lifecycle_hook.c
 *
 * PURPOSE:
 *   Implement the test lifecycle hook behavior for
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
 * File: tests/runtime_bootstrap/test_lifecycle_hook.c
 *
 * PURPOSE:
 *   Focused regression coverage for Create lifecycle hooks bound to a service and ordered runtime phase.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/lifecycle_hook.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapLifecycleHook h;
    CHECK(umi_bootstrap_lifecycle_hook_init(&h,"hook.start","svc.a",UMI_BOOTSTRAP_PHASE_START,5)==UMI_STATUS_OK);
    CHECK(h.phase==UMI_BOOTSTRAP_PHASE_START);
    return 0;
}
