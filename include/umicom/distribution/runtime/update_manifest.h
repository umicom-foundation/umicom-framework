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

/**
 * Represent the dr update manifest data shared with callers of this public contract.
 */
typedef struct UmiDrUpdateManifest { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrChannelKind channel; char package_digest[UMI_DR_DIGEST_CAPACITY]; uint64_t size_bytes; } UmiDrUpdateManifest;
/**
 * Initialise dr update manifest from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_update_manifest_init(UmiDrUpdateManifest *value);
/**
 * Check that dr update manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_update_manifest_valid(const UmiDrUpdateManifest *value);
/**
 * Provide the dr update manifest fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_update_manifest_fingerprint(const UmiDrUpdateManifest *value);

#ifdef __cplusplus
}
#endif
#endif
