/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_environment.h
 * PURPOSE: Build a deterministic child environment from a selected developer kit.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_ENVIRONMENT_H
#define UMICOM_TOOLCHAIN_KIT_ENVIRONMENT_H

#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/kit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the toolchain kit environment plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_environment_plan(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
