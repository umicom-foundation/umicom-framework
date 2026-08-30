/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_collection_binding.c
 *
 * PURPOSE:
 *   Implement the test collection binding behavior for
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
 * File: tests/runtime_bootstrap/test_collection_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for Build ordered collections of service identifiers for multi-bind dependencies.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/collection_binding.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapIdList list; umi_bootstrap_collection_binding_init(&list);
    CHECK(umi_bootstrap_collection_binding_add(&list,"svc.a")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_collection_binding_add(&list,"svc.a")==UMI_STATUS_ALREADY_EXISTS);
    CHECK(list.count==1U);
    return 0;
}
