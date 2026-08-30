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

typedef struct UmiDrPackageCatalogue { UmiDrComponentPackage items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrPackageCatalogue;
void umi_dr_package_catalogue_init(UmiDrPackageCatalogue *catalogue);
UmiStatus umi_dr_package_catalogue_add(UmiDrPackageCatalogue *catalogue, const UmiDrComponentPackage *item);
const UmiDrComponentPackage *umi_dr_package_catalogue_find(const UmiDrPackageCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
