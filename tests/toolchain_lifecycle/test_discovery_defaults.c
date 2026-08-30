/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_discovery_defaults.c
 *
 * PURPOSE:
 *   Verify every thin application can initialise the same canonical discovery
 *   policy without performing an external tool probe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/toolchain/discovery.h"

int main(void)
{
    UmiToolchainDiscoveryRequest request;

    umi_toolchain_discovery_request_init(&request);

    assert(request.explicit_root == NULL);
    assert(request.preferred_profile != NULL);
    assert(strstr(request.preferred_profile, "gcc") != NULL);
    assert(request.require_gtk == 0);
    assert(request.require_java == 0);
    assert(request.require_github_cli == 0);
    assert(request.skip_compile_probe == 0);
    assert(request.diagnostic_sink == NULL);
    assert(request.diagnostic_user_data == NULL);

    return 0;
}
