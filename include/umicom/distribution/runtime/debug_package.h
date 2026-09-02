/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/debug_package.h
 *
 * PURPOSE:
 *   diagnostic/debug companion package metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DEBUG_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_DEBUG_PACKAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr debug package data shared with callers of this public contract.
 */
typedef struct UmiDrDebugPackage { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; bool symbols; bool diagnostics; bool source_maps; } UmiDrDebugPackage;
/**
 * Initialise dr debug package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_debug_package_init(UmiDrDebugPackage *value);
/**
 * Check that dr debug package satisfies its contract before another service relies on it.
 */
bool umi_dr_debug_package_valid(const UmiDrDebugPackage *value);
/**
 * Provide the dr debug package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_debug_package_fingerprint(const UmiDrDebugPackage *value);

#ifdef __cplusplus
}
#endif
#endif
