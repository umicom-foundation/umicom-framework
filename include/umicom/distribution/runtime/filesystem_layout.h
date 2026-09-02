/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/filesystem_layout.h
 *
 * PURPOSE:
 *   canonical install-root, bin, lib, share and writable-state layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_FILESYSTEM_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_FILESYSTEM_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr filesystem layout data shared with callers of this public contract.
 */
typedef struct UmiDrFilesystemLayout { char id[UMI_DR_ID_CAPACITY]; char root[UMI_DR_PATH_CAPACITY]; char bin[UMI_DR_PATH_CAPACITY]; char lib[UMI_DR_PATH_CAPACITY]; char share[UMI_DR_PATH_CAPACITY]; char state[UMI_DR_PATH_CAPACITY]; } UmiDrFilesystemLayout;
/**
 * Initialise dr filesystem layout from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_filesystem_layout_init(UmiDrFilesystemLayout *value);
/**
 * Check that dr filesystem layout satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_filesystem_layout_valid(const UmiDrFilesystemLayout *value);
/**
 * Provide the dr filesystem layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_filesystem_layout_fingerprint(const UmiDrFilesystemLayout *value);

#ifdef __cplusplus
}
#endif
#endif
