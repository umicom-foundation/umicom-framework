/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/profile_selection.c
 *
 * PURPOSE:
 *   Implement the profile selection behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/profile_selection.h"
UmiStatus umi_project_workspace_profile_selection_best(const UmiProjectWorkspaceProfileCandidate*items,size_t count,uint64_t required,size_t*out_index) {
    size_t i,best=SIZE_MAX;
    uint32_t score=0U;
    if(items==NULL||out_index==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<count;++i) {
        if(!items[i].enabled||(items[i].capabilities&required)!=required)continue;
        if(best==SIZE_MAX||items[i].priority>score) {
            best=i;
            score=items[i].priority;
        }
    }
    if(best==SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    *out_index=best;
    return UMI_STATUS_OK;
}
