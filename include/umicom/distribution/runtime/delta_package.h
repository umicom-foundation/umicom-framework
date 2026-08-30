/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/delta_package.h
 *
 * PURPOSE:
 *   delta package base/target version and savings validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DELTA_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_DELTA_PACKAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrDeltaPackage { char id[UMI_DR_ID_CAPACITY]; UmiDrVersion base_version; UmiDrVersion target_version; uint64_t full_size; uint64_t delta_size; char digest[UMI_DR_DIGEST_CAPACITY]; } UmiDrDeltaPackage;
void umi_dr_delta_package_init(UmiDrDeltaPackage *value);
bool umi_dr_delta_package_valid(const UmiDrDeltaPackage *value);
uint64_t umi_dr_delta_package_fingerprint(const UmiDrDeltaPackage *value);

#ifdef __cplusplus
}
#endif
#endif
