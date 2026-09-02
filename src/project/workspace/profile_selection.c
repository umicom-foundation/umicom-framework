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
/*
 * Provide the project workspace profile selection best operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_profile_selection_best(const UmiProjectWorkspaceProfileCandidate*items,size_t count,uint64_t required,size_t*out_index) {
    size_t i,best=SIZE_MAX;
    uint32_t score=0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(items==NULL||out_index==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<count;++i) {
        /* Apply this operation only while the related capability or state is available. */
        if(!items[i].enabled||(items[i].capabilities&required)!=required)continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if(best==SIZE_MAX||items[i].priority>score) {
            best=i;
            score=items[i].priority;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(best==SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    *out_index=best;
    return UMI_STATUS_OK;
}
