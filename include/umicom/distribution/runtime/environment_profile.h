/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/environment_profile.h
 *
 * PURPOSE:
 *   bounded environment-variable overlay for packaged applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_ENVIRONMENT_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_ENVIRONMENT_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrEnvironmentEntry { char key[64]; char value[UMI_DR_TEXT_CAPACITY]; } UmiDrEnvironmentEntry;
typedef struct UmiDrEnvironmentProfile { char id[UMI_DR_ID_CAPACITY]; UmiDrEnvironmentEntry entries[UMI_DR_MAX_ENV]; size_t count; } UmiDrEnvironmentProfile;
void umi_dr_environment_profile_init(UmiDrEnvironmentProfile *profile); UmiStatus umi_dr_environment_profile_set(UmiDrEnvironmentProfile *profile,const char *key,const char *value); const char *umi_dr_environment_profile_get(const UmiDrEnvironmentProfile *profile,const char *key);

#ifdef __cplusplus
}
#endif
#endif
