/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_compatibility.h
 *
 * PURPOSE:
 *   runtime requirement matching with missing-capability and version evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_COMPATIBILITY_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_COMPATIBILITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/runtime_profile.h"
#include "umicom/distribution/runtime/runtime_probe.h"
/**
 * Represent the dr runtime compatibility data shared with callers of this public contract.
 */
typedef struct UmiDrRuntimeCompatibility { bool platform_match; bool architecture_match; bool version_match; uint64_t missing_capabilities; } UmiDrRuntimeCompatibility;
/**
 * Provide the dr runtime compatibility evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_dr_runtime_compatibility_evaluate(const UmiDrRuntimeProfile *required,const UmiDrRuntimeProbe *actual,UmiDrRuntimeCompatibility *out);
/**
 * Provide the dr runtime compatibility ok operation used by this module and its client
 * applications.
 */
bool umi_dr_runtime_compatibility_ok(const UmiDrRuntimeCompatibility *result);

#ifdef __cplusplus
}
#endif
#endif
