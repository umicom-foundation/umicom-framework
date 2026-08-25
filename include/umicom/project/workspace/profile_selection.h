/* Umicom Framework | Profile scoring and selection | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_PROFILE_SELECTION_H
#define UMICOM_PROJECT_WORKSPACE_PROFILE_SELECTION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProfileCandidate {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        uint64_t capabilities;
        uint32_t priority;
        bool enabled;
    }
    UmiProjectWorkspaceProfileCandidate;
    UmiStatus umi_project_workspace_profile_selection_best(const UmiProjectWorkspaceProfileCandidate*items,size_t count,uint64_t required,size_t*out_index);
#ifdef __cplusplus
}
#endif
#endif
