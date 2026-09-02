/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_capability.h
 *
 * PURPOSE:
 *   runtime capability bitsets used to gate application launches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_CAPABILITY_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_CAPABILITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr runtime capability data shared with callers of this public contract.
 */
typedef struct UmiDrRuntimeCapability { uint64_t required; uint64_t available; } UmiDrRuntimeCapability;
/**
 * Initialise dr runtime capability from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_runtime_capability_init(UmiDrRuntimeCapability *capability);
/**
 * Provide the dr runtime capability missing operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_runtime_capability_missing(const UmiDrRuntimeCapability *capability);
/**
 * Provide the dr runtime capability satisfied operation used by this module and its client
 * applications.
 */
bool umi_dr_runtime_capability_satisfied(const UmiDrRuntimeCapability *capability);

#ifdef __cplusplus
}
#endif
#endif
