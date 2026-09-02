/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/library_layout.h
 *
 * PURPOSE:
 *   shared/private runtime library placement policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_LIBRARY_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_LIBRARY_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr library layout data shared with callers of this public contract.
 */
typedef struct UmiDrLibraryLayout { char id[UMI_DR_ID_CAPACITY]; char private_dir[UMI_DR_PATH_CAPACITY]; char system_hint[UMI_DR_PATH_CAPACITY]; bool search_relative; } UmiDrLibraryLayout;
/**
 * Initialise dr library layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_library_layout_init(UmiDrLibraryLayout *value);
/**
 * Check that dr library layout satisfies its contract before another service relies on it.
 */
bool umi_dr_library_layout_valid(const UmiDrLibraryLayout *value);
/**
 * Provide the dr library layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_library_layout_fingerprint(const UmiDrLibraryLayout *value);

#ifdef __cplusplus
}
#endif
#endif
