/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_migration.h
 * PURPOSE: Framework-owned Project Migration contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_MIGRATION_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_MIGRATION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
#ifdef __cplusplus
}
#endif
#endif
