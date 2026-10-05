/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/deploy_profile.h
 *
 * PURPOSE:
 *   Publish the public deploy profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_DEPLOY_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_DEPLOY_PROFILE_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace deploy profile data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceDeployProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceDeployProfile;
    UmiStatus umi_project_workspace_deploy_profile_init(UmiProjectWorkspaceDeployProfile *value,const char *id);
    UmiStatus umi_project_workspace_deploy_profile_validate(const UmiProjectWorkspaceDeployProfile *value);
    UmiStatus umi_project_workspace_deploy_profile_set_name(UmiProjectWorkspaceDeployProfile *value,const char *name);
    UmiStatus umi_project_workspace_deploy_profile_set_detail(UmiProjectWorkspaceDeployProfile *value,const char *detail);
    UmiStatus umi_project_workspace_deploy_profile_set_state(UmiProjectWorkspaceDeployProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_deploy_profile_set_metric(UmiProjectWorkspaceDeployProfile *value,uint64_t metric);
    bool umi_project_workspace_deploy_profile_same_identity(const UmiProjectWorkspaceDeployProfile *left,const UmiProjectWorkspaceDeployProfile *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_deploy_profile_archive_encode(const UmiProjectWorkspaceDeployProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_deploy_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceDeployProfile *value);

#ifdef __cplusplus
}
#endif
#endif
