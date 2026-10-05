/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/test_profile.h
 *
 * PURPOSE:
 *   Publish the public test profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_TEST_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_TEST_PROFILE_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace test profile data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceTestProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceTestProfile;
    UmiStatus umi_project_workspace_test_profile_init(UmiProjectWorkspaceTestProfile *value,const char *id);
    UmiStatus umi_project_workspace_test_profile_validate(const UmiProjectWorkspaceTestProfile *value);
    UmiStatus umi_project_workspace_test_profile_set_name(UmiProjectWorkspaceTestProfile *value,const char *name);
    UmiStatus umi_project_workspace_test_profile_set_detail(UmiProjectWorkspaceTestProfile *value,const char *detail);
    UmiStatus umi_project_workspace_test_profile_set_state(UmiProjectWorkspaceTestProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_test_profile_set_metric(UmiProjectWorkspaceTestProfile *value,uint64_t metric);
    bool umi_project_workspace_test_profile_same_identity(const UmiProjectWorkspaceTestProfile *left,const UmiProjectWorkspaceTestProfile *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_test_profile_archive_encode(const UmiProjectWorkspaceTestProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_test_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceTestProfile *value);

#ifdef __cplusplus
}
#endif
#endif
