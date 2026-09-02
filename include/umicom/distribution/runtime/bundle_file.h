/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/bundle_file.h
 *
 * PURPOSE:
 *   individual bundle-file path, size, checksum and executable metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_FILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_FILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr bundle file data shared with callers of this public contract.
 */
typedef struct UmiDrBundleFile { char id[UMI_DR_ID_CAPACITY]; char path[UMI_DR_PATH_CAPACITY]; char digest[UMI_DR_DIGEST_CAPACITY]; uint64_t size_bytes; bool executable; } UmiDrBundleFile;
/**
 * Initialise dr bundle file from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_file_init(UmiDrBundleFile *value);
/**
 * Check that dr bundle file satisfies its contract before another service relies on it.
 */
bool umi_dr_bundle_file_valid(const UmiDrBundleFile *value);
/**
 * Provide the dr bundle file fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_bundle_file_fingerprint(const UmiDrBundleFile *value);

#ifdef __cplusplus
}
#endif
#endif
