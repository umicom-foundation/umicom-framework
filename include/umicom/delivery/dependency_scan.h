/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/dependency_scan.h
 *
 * PURPOSE:
 *   Collect runtime dependency names discovered while preparing a package.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Dependency scanning prevents a package from launching only on the developer's machine because an implicit runtime library was forgotten.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_DEPENDENCY_SCAN_H
#define INCLUDE_UMICOM_DELIVERY_DEPENDENCY_SCAN_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDependencyScan {
    char dependencies[UMI_DELIVERY_MAX_COMPONENTS][UMI_DELIVERY_ID_CAPACITY];
    size_t count;
} UmiDependencyScan;
void umi_dependency_scan_init(UmiDependencyScan *scan);
UmiStatus umi_dependency_scan_add(UmiDependencyScan *scan,
                                  const char *dependency);
int umi_dependency_scan_contains(const UmiDependencyScan *scan,
                                 const char *dependency);

#ifdef __cplusplus
}
#endif

#endif
