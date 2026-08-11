/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/dependency_scan.c
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

#include "umicom/delivery/dependency_scan.h"
#include "delivery_internal.h"
#include <string.h>

void umi_dependency_scan_init(UmiDependencyScan *scan)
{
    if (scan != NULL) (void)memset(scan, 0, sizeof(*scan));
}
UmiStatus umi_dependency_scan_add(UmiDependencyScan *scan,
                                  const char *dependency)
{
    if (scan == NULL || dependency == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (scan->count >= UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_delivery_copy_text(scan->dependencies[scan->count],
                               UMI_DELIVERY_ID_CAPACITY,
                               dependency) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++scan->count;
    return UMI_STATUS_OK;
}
int umi_dependency_scan_contains(const UmiDependencyScan *scan,
                                 const char *dependency)
{
    size_t i;
    if (scan == NULL || dependency == NULL) return 0;
    for (i = 0U; i < scan->count; ++i) {
        if (strcmp(scan->dependencies[i], dependency) == 0) return 1;
    }
    return 0;
}
