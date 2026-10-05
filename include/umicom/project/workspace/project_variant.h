/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_variant.h
 *
 * PURPOSE:
 *   Publish the public project variant contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_VARIANT_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_VARIANT_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project variant data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceProjectVariant {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectVariant;
    UmiStatus umi_project_workspace_project_variant_init(UmiProjectWorkspaceProjectVariant *value,const char *id);
    UmiStatus umi_project_workspace_project_variant_validate(const UmiProjectWorkspaceProjectVariant *value);
    UmiStatus umi_project_workspace_project_variant_set_name(UmiProjectWorkspaceProjectVariant *value,const char *name);
    UmiStatus umi_project_workspace_project_variant_set_detail(UmiProjectWorkspaceProjectVariant *value,const char *detail);
    UmiStatus umi_project_workspace_project_variant_set_state(UmiProjectWorkspaceProjectVariant *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_variant_set_metric(UmiProjectWorkspaceProjectVariant *value,uint64_t metric);
    bool umi_project_workspace_project_variant_same_identity(const UmiProjectWorkspaceProjectVariant *left,const UmiProjectWorkspaceProjectVariant *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_variant_archive_encode(const UmiProjectWorkspaceProjectVariant *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_variant_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectVariant *value);

#ifdef __cplusplus
}
#endif
#endif
