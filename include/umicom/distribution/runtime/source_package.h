/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/source_package.h
 *
 * PURPOSE:
 *   source distribution metadata, licence and reproducibility flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_SOURCE_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_SOURCE_PACKAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr source package data shared with callers of this public contract.
 */
typedef struct UmiDrSourcePackage { char id[UMI_DR_ID_CAPACITY]; char revision[UMI_DR_DIGEST_CAPACITY]; char licence[64]; bool complete; bool reproducible; } UmiDrSourcePackage;
/**
 * Initialise dr source package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_source_package_init(UmiDrSourcePackage *value);
/**
 * Check that dr source package satisfies its contract before another service relies on it.
 */
bool umi_dr_source_package_valid(const UmiDrSourcePackage *value);
/**
 * Provide the dr source package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_source_package_fingerprint(const UmiDrSourcePackage *value);

#ifdef __cplusplus
}
#endif
#endif
