/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_service_key.c
 *
 * PURPOSE:
 *   Implement the test service key behavior for
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
 * File: tests/runtime_bootstrap/test_service_key.c
 *
 * PURPOSE:
 *   Focused regression coverage for Create and compare qualified service keys used for explicit dependency lookup.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey a, b;
    CHECK(umi_bootstrap_service_key_init(&a, "svc.alpha", "primary") == UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_key_init(&b, "svc.alpha", "primary") == UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_key_equal(&a, &b));
    CHECK(umi_bootstrap_service_key_hash(&a) == umi_bootstrap_service_key_hash(&b));
    return 0;
}
