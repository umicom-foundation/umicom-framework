/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/platform_catalogue.h
 *
 * PURPOSE:
 *   bounded registry of supported operating-system targets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrPlatformCatalogue { UmiDrPlatformDescriptor items[UMI_DR_MAX_ITEMS]; size_t count; } UmiDrPlatformCatalogue;
void umi_dr_platform_catalogue_init(UmiDrPlatformCatalogue *catalogue);
UmiStatus umi_dr_platform_catalogue_add(UmiDrPlatformCatalogue *catalogue, const UmiDrPlatformDescriptor *item);
const UmiDrPlatformDescriptor *umi_dr_platform_catalogue_find(const UmiDrPlatformCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif
#endif
