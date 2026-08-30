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

typedef struct UmiDrFilesystemLayout { char id[UMI_DR_ID_CAPACITY]; char root[UMI_DR_PATH_CAPACITY]; char bin[UMI_DR_PATH_CAPACITY]; char lib[UMI_DR_PATH_CAPACITY]; char share[UMI_DR_PATH_CAPACITY]; char state[UMI_DR_PATH_CAPACITY]; } UmiDrFilesystemLayout;
void umi_dr_filesystem_layout_init(UmiDrFilesystemLayout *value);
bool umi_dr_filesystem_layout_valid(const UmiDrFilesystemLayout *value);
uint64_t umi_dr_filesystem_layout_fingerprint(const UmiDrFilesystemLayout *value);

#ifdef __cplusplus
}
#endif
#endif
