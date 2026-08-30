/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_provider_descriptor.c
 *
 * PURPOSE:
 *   Implement the test provider descriptor behavior for
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
 * File: tests/runtime_bootstrap/test_provider_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe modules that contribute replaceable service implementations.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/provider_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapProviderDescriptor d;
    CHECK(umi_bootstrap_provider_descriptor_init(&d,"provider.alpha","module.alpha",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_provider_descriptor_valid(&d));
    return 0;
}
