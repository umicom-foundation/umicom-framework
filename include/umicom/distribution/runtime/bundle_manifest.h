/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/bundle_manifest.h
 *
 * PURPOSE:
 *   bundle manifest counts, total bytes and aggregate content fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/bundle_file.h"
typedef struct UmiDrBundleManifest { size_t file_count; uint64_t total_bytes; uint64_t fingerprint; } UmiDrBundleManifest;
void umi_dr_bundle_manifest_init(UmiDrBundleManifest *manifest);
UmiStatus umi_dr_bundle_manifest_add(UmiDrBundleManifest *manifest,const UmiDrBundleFile *file);

#ifdef __cplusplus
}
#endif
#endif
