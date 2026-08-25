/* Umicom Framework | Profile Binding | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_PROFILE_BINDING_H
#define UMICOM_PROJECT_WORKSPACE_PROFILE_BINDING_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProfileBinding {
        char owner_id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char target_id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        uint64_t revision;
        bool enabled;
    }
    UmiProjectWorkspaceProfileBinding;
    UmiStatus umi_project_workspace_profile_binding_init(UmiProjectWorkspaceProfileBinding *value,const char *owner_id,const char *target_id);
    UmiStatus umi_project_workspace_profile_binding_retarget(UmiProjectWorkspaceProfileBinding *value,const char *target_id);
    bool umi_project_workspace_profile_binding_matches(const UmiProjectWorkspaceProfileBinding *value,const char *owner_id,const char *target_id);
#ifdef __cplusplus
}
#endif
#endif
