/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_scope_context.c
 *
 * PURPOSE:
 *   Implement the test scope context behavior for
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
 * File: tests/runtime_bootstrap/test_scope_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for Track nested runtime scopes without allocating application-owned containers.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/scope_context.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapScopeContext c; UmiBootstrapScopeKind out;
    umi_bootstrap_scope_context_init(&c);
    CHECK(umi_bootstrap_scope_context_push(&c,UMI_BOOTSTRAP_SCOPE_APPLICATION)==UMI_STATUS_OK);
    CHECK(c.depth==1U);
    CHECK(umi_bootstrap_scope_context_pop(&c,&out)==UMI_STATUS_OK);
    CHECK(out==UMI_BOOTSTRAP_SCOPE_APPLICATION && c.depth==0U);
    return 0;
}
