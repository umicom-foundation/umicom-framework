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

typedef struct UmiDrSourcePackage { char id[UMI_DR_ID_CAPACITY]; char revision[UMI_DR_DIGEST_CAPACITY]; char licence[64]; bool complete; bool reproducible; } UmiDrSourcePackage;
void umi_dr_source_package_init(UmiDrSourcePackage *value);
bool umi_dr_source_package_valid(const UmiDrSourcePackage *value);
uint64_t umi_dr_source_package_fingerprint(const UmiDrSourcePackage *value);

#ifdef __cplusplus
}
#endif
#endif
