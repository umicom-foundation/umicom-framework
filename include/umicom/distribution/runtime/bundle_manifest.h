/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/bundle_manifest.h
 *
 * PURPOSE:
 *   bundle manifest counts, total bytes and aggregate content fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the dr bundle manifest data shared with callers of this public contract.
 */
typedef struct UmiDrBundleManifest { size_t file_count; uint64_t total_bytes; uint64_t fingerprint; } UmiDrBundleManifest;
/**
 * Initialise dr bundle manifest from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_manifest_init(UmiDrBundleManifest *manifest);
/**
 * Add dr bundle manifest only after its inputs and available capacity have been checked.
 */
UmiStatus umi_dr_bundle_manifest_add(UmiDrBundleManifest *manifest,const UmiDrBundleFile *file);

#ifdef __cplusplus
}
#endif
#endif
