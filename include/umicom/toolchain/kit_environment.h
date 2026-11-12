/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_environment.h
 *
 * PURPOSE:
 *   Build a deterministic child environment from a selected developer kit.
 *
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
 * Validate a complete kit and stage its non-empty overrides on an optional
 * profile environment. Failure leaves out_plan unchanged. On success all
 * process entries borrow out_plan storage; keep it alive during consumption.
 * Construction can report heap allocation failure.
 */
UmiStatus umi_toolchain_kit_environment_plan(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
