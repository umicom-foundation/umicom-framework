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

typedef struct UmiDrDataLayout { char id[UMI_DR_ID_CAPACITY]; char read_only_dir[UMI_DR_PATH_CAPACITY]; char writable_dir[UMI_DR_PATH_CAPACITY]; bool migrate_legacy; } UmiDrDataLayout;
void umi_dr_data_layout_init(UmiDrDataLayout *value);
bool umi_dr_data_layout_valid(const UmiDrDataLayout *value);
uint64_t umi_dr_data_layout_fingerprint(const UmiDrDataLayout *value);

#ifdef __cplusplus
}
#endif
#endif
