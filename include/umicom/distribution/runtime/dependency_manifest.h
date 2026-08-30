/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/dependency_manifest.h
 *
 * PURPOSE:
 *   package dependency declaration with version and optionality constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DEPENDENCY_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_DEPENDENCY_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrDependencyManifest { char id[UMI_DR_ID_CAPACITY]; char package_id[UMI_DR_ID_CAPACITY]; UmiDrVersion minimum_version; bool optional; } UmiDrDependencyManifest;
void umi_dr_dependency_manifest_init(UmiDrDependencyManifest *value);
bool umi_dr_dependency_manifest_valid(const UmiDrDependencyManifest *value);
uint64_t umi_dr_dependency_manifest_fingerprint(const UmiDrDependencyManifest *value);

#ifdef __cplusplus
}
#endif
#endif
