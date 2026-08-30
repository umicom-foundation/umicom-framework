/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_scope_kind.c
 *
 * PURPOSE:
 *   Implement the test scope kind behavior for
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
 * File: tests/runtime_bootstrap/test_scope_kind.c
 *
 * PURPOSE:
 *   Focused regression coverage for Provide stable names and breadth ordering for dependency scopes.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/scope_kind.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    CHECK(strcmp(umi_bootstrap_scope_kind_text(UMI_BOOTSTRAP_SCOPE_SINGLETON),"singleton")==0);
    CHECK(umi_bootstrap_scope_kind_rank(UMI_BOOTSTRAP_SCOPE_SINGLETON) >
          umi_bootstrap_scope_kind_rank(UMI_BOOTSTRAP_SCOPE_REQUEST));
    return 0;
}
