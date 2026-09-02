/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/dependency_scan.c
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

#include "umicom/delivery/dependency_scan.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise dependency scan from caller-provided values so later operations receive a
 * known state.
 */
void umi_dependency_scan_init(UmiDependencyScan *scan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (scan != NULL) (void)memset(scan, 0, sizeof(*scan));
}
/* Add dependency scan only after its inputs and available capacity have been checked. */
UmiStatus umi_dependency_scan_add(UmiDependencyScan *scan,
                                  const char *dependency)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (scan == NULL || dependency == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (scan->count >= UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_delivery_copy_text(scan->dependencies[scan->count],
                               UMI_DELIVERY_ID_CAPACITY,
                               dependency) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++scan->count;
    return UMI_STATUS_OK;
}
/*
 * Provide the dependency scan contains operation used by this module and its client
 * applications.
 */
int umi_dependency_scan_contains(const UmiDependencyScan *scan,
                                 const char *dependency)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (scan == NULL || dependency == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < scan->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(scan->dependencies[i], dependency) == 0) return 1;
    }
    return 0;
}
