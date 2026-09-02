/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/architecture_catalogue.h
 *
 * PURPOSE:
 *   bounded architecture target registry and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_ARCHITECTURE_CATALOGUE_H
#define UMICOM_DISTRIBUTION_RUNTIME_ARCHITECTURE_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/architecture_descriptor.h"

/**
 * Represent the dr architecture catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiDrArchitectureCatalogue { UmiDrArchitectureDescriptor items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrArchitectureCatalogue;
/**
 * Initialise dr architecture catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_architecture_catalogue_init(UmiDrArchitectureCatalogue *catalogue);
/**
 * Add dr architecture catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_dr_architecture_catalogue_add(UmiDrArchitectureCatalogue *catalogue, const UmiDrArchitectureDescriptor *item);
/**
 * Find dr architecture catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDrArchitectureDescriptor *umi_dr_architecture_catalogue_find(const UmiDrArchitectureCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
