/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_configuration.h
 *
 * PURPOSE:
 *   Publish the public workspace configuration contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_CONFIGURATION_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_CONFIGURATION_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace configuration data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceConfiguration {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceWorkspaceConfiguration;
    UmiStatus umi_project_workspace_workspace_configuration_init(UmiProjectWorkspaceWorkspaceConfiguration *value,const char *id);
    UmiStatus umi_project_workspace_workspace_configuration_validate(const UmiProjectWorkspaceWorkspaceConfiguration *value);
    UmiStatus umi_project_workspace_workspace_configuration_set_name(UmiProjectWorkspaceWorkspaceConfiguration *value,const char *name);
    UmiStatus umi_project_workspace_workspace_configuration_set_detail(UmiProjectWorkspaceWorkspaceConfiguration *value,const char *detail);
    UmiStatus umi_project_workspace_workspace_configuration_set_state(UmiProjectWorkspaceWorkspaceConfiguration *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_workspace_configuration_set_metric(UmiProjectWorkspaceWorkspaceConfiguration *value,uint64_t metric);
    bool umi_project_workspace_workspace_configuration_same_identity(const UmiProjectWorkspaceWorkspaceConfiguration *left,const UmiProjectWorkspaceWorkspaceConfiguration *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_configuration_archive_encode(const UmiProjectWorkspaceWorkspaceConfiguration *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_configuration_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceConfiguration *value);

#ifdef __cplusplus
}
#endif
#endif
