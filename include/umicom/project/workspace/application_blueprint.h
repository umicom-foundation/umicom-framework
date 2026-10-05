/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_blueprint.h
 *
 * PURPOSE:
 *   Publish the public application blueprint contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_BLUEPRINT_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_BLUEPRINT_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace application blueprint data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceApplicationBlueprint {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceApplicationBlueprint;
    UmiStatus umi_project_workspace_application_blueprint_init(UmiProjectWorkspaceApplicationBlueprint *value,const char *id);
    UmiStatus umi_project_workspace_application_blueprint_validate(const UmiProjectWorkspaceApplicationBlueprint *value);
    UmiStatus umi_project_workspace_application_blueprint_set_name(UmiProjectWorkspaceApplicationBlueprint *value,const char *name);
    UmiStatus umi_project_workspace_application_blueprint_set_detail(UmiProjectWorkspaceApplicationBlueprint *value,const char *detail);
    UmiStatus umi_project_workspace_application_blueprint_set_state(UmiProjectWorkspaceApplicationBlueprint *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_application_blueprint_set_metric(UmiProjectWorkspaceApplicationBlueprint *value,uint64_t metric);
    bool umi_project_workspace_application_blueprint_same_identity(const UmiProjectWorkspaceApplicationBlueprint *left,const UmiProjectWorkspaceApplicationBlueprint *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_application_blueprint_archive_encode(const UmiProjectWorkspaceApplicationBlueprint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_application_blueprint_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceApplicationBlueprint *value);

#ifdef __cplusplus
}
#endif
#endif
