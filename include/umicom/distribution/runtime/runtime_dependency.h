/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_dependency.h
 *
 * PURPOSE:
 *   native runtime-library dependency and availability policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_DEPENDENCY_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_DEPENDENCY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrRuntimeDependency { char id[UMI_DR_ID_CAPACITY]; char soname[UMI_DR_PATH_CAPACITY]; UmiDrVersion minimum_version; bool system_provided; bool bundled; } UmiDrRuntimeDependency;
void umi_dr_runtime_dependency_init(UmiDrRuntimeDependency *value);
bool umi_dr_runtime_dependency_valid(const UmiDrRuntimeDependency *value);
uint64_t umi_dr_runtime_dependency_fingerprint(const UmiDrRuntimeDependency *value);

#ifdef __cplusplus
}
#endif
#endif
