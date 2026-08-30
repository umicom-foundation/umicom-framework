/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_capability_condition.c
 *
 * PURPOSE:
 *   Implement the test capability condition behavior for
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
 * File: tests/runtime_bootstrap/test_capability_condition.c
 *
 * PURPOSE:
 *   Focused regression coverage for Evaluate Framework runtime-capability presence.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/collection_binding.h"
#include "umicom/runtime/bootstrap/capability_condition.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapIdList l; umi_bootstrap_collection_binding_init(&l);
    CHECK(umi_bootstrap_collection_binding_add(&l,"capability.web")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_capability_condition_match(&l,"capability.web"));
    return 0;
}
