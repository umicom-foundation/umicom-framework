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

typedef struct UmiDrDebugPackage { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; bool symbols; bool diagnostics; bool source_maps; } UmiDrDebugPackage;
void umi_dr_debug_package_init(UmiDrDebugPackage *value);
bool umi_dr_debug_package_valid(const UmiDrDebugPackage *value);
uint64_t umi_dr_debug_package_fingerprint(const UmiDrDebugPackage *value);

#ifdef __cplusplus
}
#endif
#endif
