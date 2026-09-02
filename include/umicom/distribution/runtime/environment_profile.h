/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/environment_profile.h
 *
 * PURPOSE:
 *   bounded environment-variable overlay for packaged applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the dr environment entry data shared with callers of this public contract.
 */
typedef struct UmiDrEnvironmentEntry { char key[64]; char value[UMI_DR_TEXT_CAPACITY]; } UmiDrEnvironmentEntry;
/**
 * Represent the dr environment profile data shared with callers of this public contract.
 */
typedef struct UmiDrEnvironmentProfile { char id[UMI_DR_ID_CAPACITY]; UmiDrEnvironmentEntry entries[UMI_DR_MAX_ENV]; size_t count; } UmiDrEnvironmentProfile;
/**
 * Initialise dr environment profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_environment_profile_init(UmiDrEnvironmentProfile *profile); UmiStatus umi_dr_environment_profile_set(UmiDrEnvironmentProfile *profile,const char *key,const char *value); const char *umi_dr_environment_profile_get(const UmiDrEnvironmentProfile *profile,const char *key);

#ifdef __cplusplus
}
#endif
#endif
