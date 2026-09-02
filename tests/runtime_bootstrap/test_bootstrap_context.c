/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_context.c
 *
 * PURPOSE:
 *   Implement the test bootstrap context behavior for
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
 * File: tests/runtime_bootstrap/test_bootstrap_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for Create the complete application bootstrap input owned by Framework runtime.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_context.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapContext c;
    CHECK(umi_bootstrap_context_init(&c,"app.studio","windows",true)==UMI_STATUS_OK);
    CHECK(c.dry_run && strcmp(c.application_id,"app.studio")==0);
    return 0;
}
