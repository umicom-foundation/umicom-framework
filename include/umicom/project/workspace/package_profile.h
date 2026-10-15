/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/package_profile.h
 *
 * PURPOSE:
 *   Publish the public package profile contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PACKAGE_PROFILE_H
#define UMICOM_PROJECT_WORKSPACE_PACKAGE_PROFILE_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace package profile data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspacePackageProfile {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspacePackageProfile;
    /** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_project_workspace_package_profile_init(UmiProjectWorkspacePackageProfile *value,const char *id);
    UmiStatus umi_project_workspace_package_profile_validate(const UmiProjectWorkspacePackageProfile *value);
    UmiStatus umi_project_workspace_package_profile_set_name(UmiProjectWorkspacePackageProfile *value,const char *name);
    UmiStatus umi_project_workspace_package_profile_set_detail(UmiProjectWorkspacePackageProfile *value,const char *detail);
    UmiStatus umi_project_workspace_package_profile_set_state(UmiProjectWorkspacePackageProfile *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_package_profile_set_metric(UmiProjectWorkspacePackageProfile *value,uint64_t metric);
    bool umi_project_workspace_package_profile_same_identity(const UmiProjectWorkspacePackageProfile *left,const UmiProjectWorkspacePackageProfile *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_package_profile_archive_encode(const UmiProjectWorkspacePackageProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_package_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspacePackageProfile *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_project_workspace_package_profile_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiProjectWorkspacePackageProfile *expected, UmiProjectWorkspacePackageProfile *value);


#ifdef __cplusplus
}
#endif
#endif
