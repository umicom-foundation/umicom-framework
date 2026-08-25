/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/library_layout.h
 *
 * PURPOSE:
 *   shared/private runtime library placement policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrLibraryLayout { char id[UMI_DR_ID_CAPACITY]; char private_dir[UMI_DR_PATH_CAPACITY]; char system_hint[UMI_DR_PATH_CAPACITY]; bool search_relative; } UmiDrLibraryLayout;
void umi_dr_library_layout_init(UmiDrLibraryLayout *value);
bool umi_dr_library_layout_valid(const UmiDrLibraryLayout *value);
uint64_t umi_dr_library_layout_fingerprint(const UmiDrLibraryLayout *value);

#ifdef __cplusplus
}
#endif
#endif
