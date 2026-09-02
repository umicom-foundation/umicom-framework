/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/scoped_environment.h
 *
 * PURPOSE:
 *   Create child-process environments from the tools actually used by an operation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_SCOPED_ENVIRONMENT_H
#define INCLUDE_UMICOM_TOOLCHAIN_SCOPED_ENVIRONMENT_H
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/operation_profile.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the toolchain scoped environment operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_scoped_environment(
    const UmiToolchainProfile *profile,
    const UmiToolchainOperationProfile *operation,
    UmiEnvironmentPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
