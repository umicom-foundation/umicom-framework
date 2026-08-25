/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/resource_manifest.h
 *
 * PURPOSE:
 *   resource-pack identity, locale, scale and content metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrResourceManifest { char id[UMI_DR_ID_CAPACITY]; char locale[32]; uint32_t scale_percent; uint64_t size_bytes; char digest[UMI_DR_DIGEST_CAPACITY]; } UmiDrResourceManifest;
void umi_dr_resource_manifest_init(UmiDrResourceManifest *value);
bool umi_dr_resource_manifest_valid(const UmiDrResourceManifest *value);
uint64_t umi_dr_resource_manifest_fingerprint(const UmiDrResourceManifest *value);

#ifdef __cplusplus
}
#endif
#endif
