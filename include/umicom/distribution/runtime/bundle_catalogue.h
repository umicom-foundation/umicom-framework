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

/**
 * Represent the dr bundle catalogue data shared with callers of this public contract.
 */
typedef struct UmiDrBundleCatalogue { UmiDrApplicationBundle items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrBundleCatalogue;
/**
 * Initialise dr bundle catalogue from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_catalogue_init(UmiDrBundleCatalogue *catalogue);
/**
 * Add dr bundle catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_dr_bundle_catalogue_add(UmiDrBundleCatalogue *catalogue, const UmiDrApplicationBundle *item);
/**
 * Find dr bundle catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDrApplicationBundle *umi_dr_bundle_catalogue_find(const UmiDrBundleCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
