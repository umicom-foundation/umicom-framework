/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/provider.h
 *
 * PURPOSE:
 *   Define provider-neutral command generation for configure, build, test, clean and run phases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PROVIDER_H
#define UMICOM_BUILD_PROVIDER_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/profile.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiBuildProviderCommandFn)(
    const UmiBuildProfile *profile,
    UmiBuildPhase phase,
    UmiBuildCommand *out_command
);

typedef struct UmiBuildProvider {
    uint32_t structure_size;
    const char *provider_id;
    uint32_t supported_phases;
    UmiBuildProviderCommandFn create_command;
} UmiBuildProvider;

#define UMI_BUILD_PHASE_MASK(phase) (1U << (unsigned)(phase))

int umi_build_provider_supports(const UmiBuildProvider *provider,
                                UmiBuildPhase phase);
UmiStatus umi_build_provider_create_command(
    const UmiBuildProvider *provider,
    const UmiBuildProfile *profile,
    UmiBuildPhase phase,
    UmiBuildCommand *out_command
);

#ifdef __cplusplus
}
#endif

#endif
