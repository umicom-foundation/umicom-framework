/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_health.h
 *
 * PURPOSE:
 *   Publish the public workspace health contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_HEALTH_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_HEALTH_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace health data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceHealth {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceHealth;
    UmiStatus umi_project_workspace_workspace_health_init(UmiProjectWorkspaceWorkspaceHealth *value,const char *id);
    UmiStatus umi_project_workspace_workspace_health_validate(const UmiProjectWorkspaceWorkspaceHealth *value);
    UmiStatus umi_project_workspace_workspace_health_set_name(UmiProjectWorkspaceWorkspaceHealth *value,const char *name);
    UmiStatus umi_project_workspace_workspace_health_set_detail(UmiProjectWorkspaceWorkspaceHealth *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_health_set_state(UmiProjectWorkspaceWorkspaceHealth *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_health_set_metric(UmiProjectWorkspaceWorkspaceHealth *value,uint64_t metric);
    bool umi_project_workspace_workspace_health_same_identity(const UmiProjectWorkspaceWorkspaceHealth *left,const UmiProjectWorkspaceWorkspaceHealth *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_health_archive_encode(const UmiProjectWorkspaceWorkspaceHealth *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_health_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceHealth *value);

#ifdef __cplusplus
}
#endif
#endif
