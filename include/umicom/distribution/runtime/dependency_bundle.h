/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/dependency_bundle.h
 *
 * PURPOSE:
 *   resolved dependency set for one application release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DEPENDENCY_BUNDLE_H
#define UMICOM_DISTRIBUTION_RUNTIME_DEPENDENCY_BUNDLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr dependency bundle data shared with callers of this public contract.
 */
typedef struct UmiDrDependencyBundle { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; size_t resolved_count; size_t optional_missing; uint64_t fingerprint; } UmiDrDependencyBundle;
/**
 * Initialise dr dependency bundle from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_dependency_bundle_init(UmiDrDependencyBundle *value);
/**
 * Check that dr dependency bundle satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_dependency_bundle_valid(const UmiDrDependencyBundle *value);
/**
 * Provide the dr dependency bundle fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_dependency_bundle_fingerprint(const UmiDrDependencyBundle *value);

#ifdef __cplusplus
}
#endif
#endif
