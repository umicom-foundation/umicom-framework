/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/update_manifest.h
 *
 * PURPOSE:
 *   published update metadata, version, channel and package fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_UPDATE_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_UPDATE_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrUpdateManifest { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrChannelKind channel; char package_digest[UMI_DR_DIGEST_CAPACITY]; uint64_t size_bytes; } UmiDrUpdateManifest;
void umi_dr_update_manifest_init(UmiDrUpdateManifest *value);
bool umi_dr_update_manifest_valid(const UmiDrUpdateManifest *value);
uint64_t umi_dr_update_manifest_fingerprint(const UmiDrUpdateManifest *value);

#ifdef __cplusplus
}
#endif
#endif
