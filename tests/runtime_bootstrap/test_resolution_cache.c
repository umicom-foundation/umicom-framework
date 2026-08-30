/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_resolution_cache.c
 *
 * PURPOSE:
 *   Implement the test resolution cache behavior for
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
 * File: tests/runtime_bootstrap/test_resolution_cache.c
 *
 * PURPOSE:
 *   Focused regression coverage for Cache resolved instances by qualified service key within an allowed scope.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/resolution_cache.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapResolutionCache c; int value=3;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.a","")==UMI_STATUS_OK);
    umi_bootstrap_resolution_cache_init(&c);
    CHECK(umi_bootstrap_resolution_cache_put(&c,&key,&value)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_resolution_cache_find(&c,&key)==&value);
    return 0;
}
