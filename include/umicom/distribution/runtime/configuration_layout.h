/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/configuration_layout.h
 *
 * PURPOSE:
 *   system, user and portable configuration-root policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CONFIGURATION_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_CONFIGURATION_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrConfigurationLayout { char id[UMI_DR_ID_CAPACITY]; char system_root[UMI_DR_PATH_CAPACITY]; char user_root[UMI_DR_PATH_CAPACITY]; bool portable; } UmiDrConfigurationLayout;
void umi_dr_configuration_layout_init(UmiDrConfigurationLayout *value);
bool umi_dr_configuration_layout_valid(const UmiDrConfigurationLayout *value);
uint64_t umi_dr_configuration_layout_fingerprint(const UmiDrConfigurationLayout *value);

#ifdef __cplusplus
}
#endif
#endif
