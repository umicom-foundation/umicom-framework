/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_activation.h
 *
 * PURPOSE:
 *   Publish the public project activation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_ACTIVATION_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_ACTIVATION_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project activation data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceProjectActivation {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectActivation;
    UmiStatus umi_project_workspace_project_activation_init(UmiProjectWorkspaceProjectActivation *value,const char *id);
    UmiStatus umi_project_workspace_project_activation_validate(const UmiProjectWorkspaceProjectActivation *value);
    UmiStatus umi_project_workspace_project_activation_set_name(UmiProjectWorkspaceProjectActivation *value,const char *name);
    UmiStatus umi_project_workspace_project_activation_set_detail(UmiProjectWorkspaceProjectActivation *value,const char *detail);
    UmiStatus umi_project_workspace_project_activation_set_state(UmiProjectWorkspaceProjectActivation *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_activation_set_metric(UmiProjectWorkspaceProjectActivation *value,uint64_t metric);
    bool umi_project_workspace_project_activation_same_identity(const UmiProjectWorkspaceProjectActivation *left,const UmiProjectWorkspaceProjectActivation *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_activation_archive_encode(const UmiProjectWorkspaceProjectActivation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_activation_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectActivation *value);

#ifdef __cplusplus
}
#endif
#endif
