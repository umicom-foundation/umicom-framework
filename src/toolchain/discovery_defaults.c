/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/discovery_defaults.c
 *
 * PURPOSE:
 *   Provide one reusable default toolchain-discovery policy for Studio, future
 *   thin applications, CLI tools and automation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/discovery.h"

#include <string.h>

void umi_toolchain_discovery_request_init(
    UmiToolchainDiscoveryRequest *request)
{
    if (request == NULL) {
        return;
    }

    (void)memset(request, 0, sizeof(*request));

#ifdef _WIN32
    /*
     * The Umicom Windows baseline is MSYS2 UCRT64 + GCC. Clang and MSVC remain
     * supported explicit profiles, but a zero-configuration consumer should
     * discover the same compiler family used by the canonical project preset.
     */
    request->preferred_profile = "windows-ucrt64-gcc";
#else
    request->preferred_profile = "posix-gcc";
#endif
}
