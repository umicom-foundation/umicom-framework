/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/platform_catalogue.h
 *
 * PURPOSE:
 *   bounded registry of supported operating-system targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PLATFORM_CATALOGUE_H
#define UMICOM_DISTRIBUTION_RUNTIME_PLATFORM_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/platform_descriptor.h"

/**
 * Represent the dr platform catalogue data shared with callers of this public contract.
 */
typedef struct UmiDrPlatformCatalogue { UmiDrPlatformDescriptor items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrPlatformCatalogue;
/**
 * Initialise dr platform catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_platform_catalogue_init(UmiDrPlatformCatalogue *catalogue);
/**
 * Add dr platform catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_dr_platform_catalogue_add(UmiDrPlatformCatalogue *catalogue, const UmiDrPlatformDescriptor *item);
/**
 * Find dr platform catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDrPlatformDescriptor *umi_dr_platform_catalogue_find(const UmiDrPlatformCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
