/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/executable_layout.h
 *
 * PURPOSE:
 *   executable placement and launch-entry validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_EXECUTABLE_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_EXECUTABLE_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrExecutableLayout { char id[UMI_DR_ID_CAPACITY]; char entrypoint[UMI_DR_PATH_CAPACITY]; char bin_dir[UMI_DR_PATH_CAPACITY]; bool console; bool gui; } UmiDrExecutableLayout;
void umi_dr_executable_layout_init(UmiDrExecutableLayout *value);
bool umi_dr_executable_layout_valid(const UmiDrExecutableLayout *value);
uint64_t umi_dr_executable_layout_fingerprint(const UmiDrExecutableLayout *value);

#ifdef __cplusplus
}
#endif
#endif
