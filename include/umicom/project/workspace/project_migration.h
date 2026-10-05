/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_migration.h
 *
 * PURPOSE:
 *   Publish the public project migration contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_MIGRATION_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_MIGRATION_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project migration data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceProjectMigration {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectMigration;
    UmiStatus umi_project_workspace_project_migration_init(UmiProjectWorkspaceProjectMigration *value,const char *id);
    UmiStatus umi_project_workspace_project_migration_validate(const UmiProjectWorkspaceProjectMigration *value);
    UmiStatus umi_project_workspace_project_migration_set_name(UmiProjectWorkspaceProjectMigration *value,const char *name);
    UmiStatus umi_project_workspace_project_migration_set_detail(UmiProjectWorkspaceProjectMigration *value,const char *detail);
    UmiStatus umi_project_workspace_project_migration_set_state(UmiProjectWorkspaceProjectMigration *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_migration_set_metric(UmiProjectWorkspaceProjectMigration *value,uint64_t metric);
    bool umi_project_workspace_project_migration_same_identity(const UmiProjectWorkspaceProjectMigration *left,const UmiProjectWorkspaceProjectMigration *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_migration_archive_encode(const UmiProjectWorkspaceProjectMigration *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_migration_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectMigration *value);

#ifdef __cplusplus
}
#endif
#endif
