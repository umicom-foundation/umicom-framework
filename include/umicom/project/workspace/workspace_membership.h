/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_membership.h
 *
 * PURPOSE:
 *   Publish the public workspace membership contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_MEMBERSHIP_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_MEMBERSHIP_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace membership data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceMembership {
        char owner_id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char target_id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        uint64_t revision;
        bool enabled;
    }
    UmiProjectWorkspaceWorkspaceMembership;
    /** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_project_workspace_workspace_membership_init(UmiProjectWorkspaceWorkspaceMembership *value,const char *owner_id,const char *target_id);
    UmiStatus umi_project_workspace_workspace_membership_retarget(UmiProjectWorkspaceWorkspaceMembership *value,const char *target_id);
    bool umi_project_workspace_workspace_membership_matches(const UmiProjectWorkspaceWorkspaceMembership *value,const char *owner_id,const char *target_id);
#ifdef __cplusplus
}
#endif
#endif
