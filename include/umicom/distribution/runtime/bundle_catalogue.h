/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/bundle_catalogue.h
 *
 * PURPOSE:
 *   bounded bundle inventory with duplicate rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_CATALOGUE_H
#define UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/application_bundle.h"

typedef struct UmiDrBundleCatalogue { UmiDrApplicationBundle items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrBundleCatalogue;
void umi_dr_bundle_catalogue_init(UmiDrBundleCatalogue *catalogue);
UmiStatus umi_dr_bundle_catalogue_add(UmiDrBundleCatalogue *catalogue, const UmiDrApplicationBundle *item);
const UmiDrApplicationBundle *umi_dr_bundle_catalogue_find(const UmiDrBundleCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
