/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_FRONTEND_BUILD_PROFILE_H
#define UMICOM_FRONTEND_BUILD_PROFILE_H
#include "umicom/frontend/dev_types.h"
typedef struct UmiFrontendBuildProfile { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char name[UMI_FRONTEND_DEV_NAME_CAPACITY]; char source_root[UMI_FRONTEND_DEV_PATH_CAPACITY]; char entry_file[UMI_FRONTEND_DEV_PATH_CAPACITY]; char output_directory[UMI_FRONTEND_DEV_PATH_CAPACITY]; char browser_targets[UMI_FRONTEND_DEV_NAME_CAPACITY]; int production; int minify; int source_maps; int strict; } UmiFrontendBuildProfile;
typedef struct UmiFrontendBuildProfiles { UmiFrontendBuildProfile items[UMI_FRONTEND_DEV_MAX_BUILD_PROFILES]; size_t count; char active_id[UMI_FRONTEND_DEV_ID_CAPACITY]; uint64_t revision; } UmiFrontendBuildProfiles;
UmiStatus umi_frontend_build_profiles_init(UmiFrontendBuildProfiles *profiles);
UmiStatus umi_frontend_build_profiles_add(UmiFrontendBuildProfiles *profiles,const UmiFrontendBuildProfile *profile);
UmiStatus umi_frontend_build_profiles_activate(UmiFrontendBuildProfiles *profiles,const char *id);
UmiStatus umi_frontend_build_profiles_active(const UmiFrontendBuildProfiles *profiles,UmiFrontendBuildProfile *out_profile);
#endif
