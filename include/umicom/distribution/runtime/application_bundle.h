/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/application_bundle.h
 *
 * PURPOSE:
 *   application bundle metadata, selected variant and immutable content fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_BUNDLE_H
#define UMICOM_DISTRIBUTION_RUNTIME_APPLICATION_BUNDLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr application bundle data shared with callers of this public contract.
 */
typedef struct UmiDrApplicationBundle { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; char variant_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; uint64_t content_fingerprint; size_t file_count; } UmiDrApplicationBundle;
/**
 * Initialise dr application bundle from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_application_bundle_init(UmiDrApplicationBundle *value);
/**
 * Check that dr application bundle satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_application_bundle_valid(const UmiDrApplicationBundle *value);
/**
 * Provide the dr application bundle fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_bundle_fingerprint(const UmiDrApplicationBundle *value);

#ifdef __cplusplus
}
#endif
#endif
