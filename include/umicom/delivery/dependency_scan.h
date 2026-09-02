/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/dependency_scan.h
 *
 * PURPOSE:
 *   Collect runtime dependency names discovered while preparing a package.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Dependency scanning prevents a package from launching only on the developer's machine because an implicit runtime library was forgotten.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_DEPENDENCY_SCAN_H
#define INCLUDE_UMICOM_DELIVERY_DEPENDENCY_SCAN_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dependency scan data shared with callers of this public contract.
 */
typedef struct UmiDependencyScan {
    char dependencies[UMI_DELIVERY_MAX_COMPONENTS][UMI_DELIVERY_ID_CAPACITY];
    size_t count;
} UmiDependencyScan;
/**
 * Initialise dependency scan from caller-provided values so later operations receive a
 * known state.
 */
void umi_dependency_scan_init(UmiDependencyScan *scan);
/**
 * Add dependency scan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_dependency_scan_add(UmiDependencyScan *scan,
                                  const char *dependency);
/**
 * Provide the dependency scan contains operation used by this module and its client
 * applications.
 */
int umi_dependency_scan_contains(const UmiDependencyScan *scan,
                                 const char *dependency);

#ifdef __cplusplus
}
#endif

#endif
