/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/architecture_catalogue.h
 *
 * PURPOSE:
 *   bounded architecture target registry and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrArchitectureCatalogue { UmiDrArchitectureDescriptor items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrArchitectureCatalogue;
void umi_dr_architecture_catalogue_init(UmiDrArchitectureCatalogue *catalogue);
UmiStatus umi_dr_architecture_catalogue_add(UmiDrArchitectureCatalogue *catalogue, const UmiDrArchitectureDescriptor *item);
const UmiDrArchitectureDescriptor *umi_dr_architecture_catalogue_find(const UmiDrArchitectureCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
