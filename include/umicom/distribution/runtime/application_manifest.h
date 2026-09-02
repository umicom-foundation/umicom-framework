/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_manifest.h
 *
 * PURPOSE:
 *   cross-platform application identity and runtime requirement manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_MANIFEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr application manifest data shared with callers of this public contract.
 */
typedef struct UmiDrApplicationManifest { char id[UMI_DR_ID_CAPACITY]; char name[UMI_DR_TEXT_CAPACITY]; UmiDrVersion version; uint64_t required_capabilities; bool gui; bool networked; } UmiDrApplicationManifest;
/**
 * Initialise dr application manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_manifest_init(UmiDrApplicationManifest *value);
/**
 * Check that dr application manifest satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_manifest_valid(const UmiDrApplicationManifest *value);
/**
 * Provide the dr application manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_manifest_fingerprint(const UmiDrApplicationManifest *value);

#ifdef __cplusplus
}
#endif
#endif
