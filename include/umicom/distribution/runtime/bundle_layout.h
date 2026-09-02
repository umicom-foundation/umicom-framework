/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/bundle_layout.h
 *
 * PURPOSE:
 *   portable application bundle directory layout validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_BUNDLE_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr bundle layout data shared with callers of this public contract.
 */
typedef struct UmiDrBundleLayout { char id[UMI_DR_ID_CAPACITY]; char bin_dir[UMI_DR_PATH_CAPACITY]; char lib_dir[UMI_DR_PATH_CAPACITY]; char share_dir[UMI_DR_PATH_CAPACITY]; char state_dir[UMI_DR_PATH_CAPACITY]; } UmiDrBundleLayout;
/**
 * Initialise dr bundle layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_layout_init(UmiDrBundleLayout *value);
/**
 * Check that dr bundle layout satisfies its contract before another service relies on it.
 */
bool umi_dr_bundle_layout_valid(const UmiDrBundleLayout *value);
/**
 * Provide the dr bundle layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_bundle_layout_fingerprint(const UmiDrBundleLayout *value);

#ifdef __cplusplus
}
#endif
#endif
