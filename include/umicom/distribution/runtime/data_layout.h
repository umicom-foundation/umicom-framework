/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/data_layout.h
 *
 * PURPOSE:
 *   read-only packaged data and writable application-data separation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DATA_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_DATA_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr data layout data shared with callers of this public contract.
 */
typedef struct UmiDrDataLayout { char id[UMI_DR_ID_CAPACITY]; char read_only_dir[UMI_DR_PATH_CAPACITY]; char writable_dir[UMI_DR_PATH_CAPACITY]; bool migrate_legacy; } UmiDrDataLayout;
/**
 * Initialise dr data layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_data_layout_init(UmiDrDataLayout *value);
/**
 * Check that dr data layout satisfies its contract before another service relies on it.
 */
bool umi_dr_data_layout_valid(const UmiDrDataLayout *value);
/**
 * Provide the dr data layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_data_layout_fingerprint(const UmiDrDataLayout *value);

#ifdef __cplusplus
}
#endif
#endif
