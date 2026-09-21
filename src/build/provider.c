/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/provider.c
 *
 * PURPOSE:
 *   Implement provider capability checks and validated command dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/provider.h"
/* The former upper bound was UMI_BUILD_PHASE_INSTALL. Package, Rebuild
 * and local Deploy are now declared by include/umicom/build/types.h. */

int umi_build_provider_supports(const UmiBuildProvider *provider,
                                UmiBuildPhase phase)
{
    if (provider == NULL || phase < UMI_BUILD_PHASE_CONFIGURE ||
        phase > UMI_BUILD_PHASE_DEPLOY) {
        return 0;
    }
    return (provider->supported_phases &
            UMI_BUILD_PHASE_MASK(phase)) != 0U;
}

UmiStatus umi_build_provider_create_command(
    const UmiBuildProvider *provider,
    const UmiBuildProfile *profile,
    UmiBuildPhase phase,
    UmiBuildCommand *out_command)
{
    if (provider == NULL || profile == NULL || out_command == NULL ||
        provider->create_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_build_provider_supports(provider, phase)) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return provider->create_command(profile, phase, out_command);
}

// MIGRATION REFERENCE — previous implementation excerpts
// The shared build session now distinguishes Package, Rebuild and local Deploy. Command execution stays in src/build/runner.c and its existing providers; CPack uses src/build/cpack_provider.c.
// These comments explain superseded statements; do not enable both execution paths.
// Previous source near line 22:
//         phase > UMI_BUILD_PHASE_INSTALL) {
