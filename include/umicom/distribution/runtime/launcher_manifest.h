/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/launcher_manifest.h
 *
 * PURPOSE:
 *   launcher executable, arguments and working-directory contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_LAUNCHER_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_LAUNCHER_MANIFEST_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr launcher manifest data shared with callers of this public contract.
 */
typedef struct UmiDrLauncherManifest { char id[UMI_DR_ID_CAPACITY]; char executable[UMI_DR_PATH_CAPACITY]; char working_directory[UMI_DR_PATH_CAPACITY]; char arguments[UMI_DR_TEXT_CAPACITY]; bool single_instance; } UmiDrLauncherManifest;
/**
 * Initialise dr launcher manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_launcher_manifest_init(UmiDrLauncherManifest *value);
/**
 * Check that dr launcher manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_launcher_manifest_valid(const UmiDrLauncherManifest *value);
/**
 * Provide the dr launcher manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_launcher_manifest_fingerprint(const UmiDrLauncherManifest *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_launcher_manifest_archive_encode(const UmiDrLauncherManifest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_launcher_manifest_archive_decode(const void *bytes, size_t byte_count,
    UmiDrLauncherManifest *value);

#ifdef __cplusplus
}
#endif
#endif
