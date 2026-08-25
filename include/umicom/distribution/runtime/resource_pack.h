/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/resource_pack.h
 *
 * PURPOSE:
 *   bounded resource-pack entries and aggregate size accounting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_PACK_H
#define UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_PACK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/resource_manifest.h"
typedef struct UmiDrResourcePack { UmiDrResourceManifest items[UMI_DR_MAX_ITEMS]; size_t count; uint64_t total_bytes; } UmiDrResourcePack;
void umi_dr_resource_pack_init(UmiDrResourcePack *pack); UmiStatus umi_dr_resource_pack_add(UmiDrResourcePack *pack,const UmiDrResourceManifest *item);

#ifdef __cplusplus
}
#endif
#endif
