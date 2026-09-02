/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/build_profile.h
 *
 * PURPOSE:
 *   Publish the public build profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_BUILD_PROFILE_H
#define UMICOM_FRONTEND_BUILD_PROFILE_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend build profile data shared with callers of this public contract.
 */
typedef struct UmiFrontendBuildProfile { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char name[UMI_FRONTEND_DEV_NAME_CAPACITY]; char source_root[UMI_FRONTEND_DEV_PATH_CAPACITY]; char entry_file[UMI_FRONTEND_DEV_PATH_CAPACITY]; char output_directory[UMI_FRONTEND_DEV_PATH_CAPACITY]; char browser_targets[UMI_FRONTEND_DEV_NAME_CAPACITY]; int production; int minify; int source_maps; int strict; } UmiFrontendBuildProfile;
/**
 * Represent the frontend build profiles data shared with callers of this public contract.
 */
typedef struct UmiFrontendBuildProfiles { UmiFrontendBuildProfile items[UMI_FRONTEND_DEV_MAX_BUILD_PROFILES]; size_t count; char active_id[UMI_FRONTEND_DEV_ID_CAPACITY]; uint64_t revision; } UmiFrontendBuildProfiles;
/**
 * Initialise frontend build profiles from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_build_profiles_init(UmiFrontendBuildProfiles *profiles);
/**
 * Add frontend build profiles only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_frontend_build_profiles_add(UmiFrontendBuildProfiles *profiles,const UmiFrontendBuildProfile *profile);
/**
 * Provide the frontend build profiles activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_build_profiles_activate(UmiFrontendBuildProfiles *profiles,const char *id);
/**
 * Provide the frontend build profiles active operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_build_profiles_active(const UmiFrontendBuildProfiles *profiles,UmiFrontendBuildProfile *out_profile);
#endif
