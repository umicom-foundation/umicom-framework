/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/release_manifest.h
 *
 * PURPOSE:
 *   release identity, channel, platform matrix and artifact summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RELEASE_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_RELEASE_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrReleaseManifest { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrChannelKind channel; size_t artifact_count; size_t platform_count; uint64_t fingerprint; } UmiDrReleaseManifest;
void umi_dr_release_manifest_init(UmiDrReleaseManifest *value);
bool umi_dr_release_manifest_valid(const UmiDrReleaseManifest *value);
uint64_t umi_dr_release_manifest_fingerprint(const UmiDrReleaseManifest *value);

#ifdef __cplusplus
}
#endif
#endif
