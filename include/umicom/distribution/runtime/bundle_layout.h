/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/bundle_layout.h
 *
 * PURPOSE:
 *   portable application bundle directory layout validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrBundleLayout { char id[UMI_DR_ID_CAPACITY]; char bin_dir[UMI_DR_PATH_CAPACITY]; char lib_dir[UMI_DR_PATH_CAPACITY]; char share_dir[UMI_DR_PATH_CAPACITY]; char state_dir[UMI_DR_PATH_CAPACITY]; } UmiDrBundleLayout;
void umi_dr_bundle_layout_init(UmiDrBundleLayout *value);
bool umi_dr_bundle_layout_valid(const UmiDrBundleLayout *value);
uint64_t umi_dr_bundle_layout_fingerprint(const UmiDrBundleLayout *value);

#ifdef __cplusplus
}
#endif
#endif
