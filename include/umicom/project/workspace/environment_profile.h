/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/environment_profile.h
 *
 * PURPOSE:
 *   Publish the public environment profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_ENVIRONMENT_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_ENVIRONMENT_PROFILE_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace environment profile data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceEnvironmentProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceEnvironmentProfile;
    UmiStatus umi_project_workspace_environment_profile_init(UmiProjectWorkspaceEnvironmentProfile *value,const char *id);
    UmiStatus umi_project_workspace_environment_profile_validate(const UmiProjectWorkspaceEnvironmentProfile *value);
    UmiStatus umi_project_workspace_environment_profile_set_name(UmiProjectWorkspaceEnvironmentProfile *value,const char *name);
    UmiStatus umi_project_workspace_environment_profile_set_detail(UmiProjectWorkspaceEnvironmentProfile *value,const char *detail);
    UmiStatus umi_project_workspace_environment_profile_set_state(UmiProjectWorkspaceEnvironmentProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_environment_profile_set_metric(UmiProjectWorkspaceEnvironmentProfile *value,uint64_t metric);
    bool umi_project_workspace_environment_profile_same_identity(const UmiProjectWorkspaceEnvironmentProfile *left,const UmiProjectWorkspaceEnvironmentProfile *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_environment_profile_archive_encode(const UmiProjectWorkspaceEnvironmentProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_environment_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceEnvironmentProfile *value);

#ifdef __cplusplus
}
#endif
#endif
