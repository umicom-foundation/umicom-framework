/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/package_catalogue.h
 *
 * PURPOSE:
 *   bounded package catalogue with name/version/target uniqueness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_CATALOGUE_H
#define UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/component_package.h"

/**
 * Represent the dr package catalogue data shared with callers of this public contract.
 */
typedef struct UmiDrPackageCatalogue { UmiDrComponentPackage items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrPackageCatalogue;
/**
 * Initialise dr package catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_package_catalogue_init(UmiDrPackageCatalogue *catalogue);
/**
 * Add dr package catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_dr_package_catalogue_add(UmiDrPackageCatalogue *catalogue, const UmiDrComponentPackage *item);
/**
 * Find dr package catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDrComponentPackage *umi_dr_package_catalogue_find(const UmiDrPackageCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
