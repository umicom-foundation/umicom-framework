/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/launch_profile.h
 *
 * PURPOSE:
 *   named launch profile with environment, frontend and safe-mode controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_LAUNCH_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_LAUNCH_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrLaunchProfile { char id[UMI_DR_ID_CAPACITY]; char launcher_id[UMI_DR_ID_CAPACITY]; char environment_id[UMI_DR_ID_CAPACITY]; char frontend[32]; bool safe_mode; bool offline; } UmiDrLaunchProfile;
void umi_dr_launch_profile_init(UmiDrLaunchProfile *value);
bool umi_dr_launch_profile_valid(const UmiDrLaunchProfile *value);
uint64_t umi_dr_launch_profile_fingerprint(const UmiDrLaunchProfile *value);

#ifdef __cplusplus
}
#endif
#endif
