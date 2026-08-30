/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_migration_plan.h
 *
 * PURPOSE:
 *   Publish the public workspace migration plan contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_MIGRATION_PLAN_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_MIGRATION_PLAN_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceMigrationPlanStep {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char action[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        bool required;
        bool completed;
    }
    UmiProjectWorkspaceWorkspaceMigrationPlanStep;
    typedef struct UmiProjectWorkspaceWorkspaceMigrationPlan {
        UmiProjectWorkspaceWorkspaceMigrationPlanStep steps[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY];
        size_t count;
        size_t completed_count;
        uint64_t revision;
    }
    UmiProjectWorkspaceWorkspaceMigrationPlan;
    void umi_project_workspace_workspace_migration_plan_init(UmiProjectWorkspaceWorkspaceMigrationPlan *value);
    UmiStatus umi_project_workspace_workspace_migration_plan_add(UmiProjectWorkspaceWorkspaceMigrationPlan *value,const char *id,const char *action,bool required);
    UmiStatus umi_project_workspace_workspace_migration_plan_complete(UmiProjectWorkspaceWorkspaceMigrationPlan *value,const char *id);
    bool umi_project_workspace_workspace_migration_plan_ready(const UmiProjectWorkspaceWorkspaceMigrationPlan *value);
#ifdef __cplusplus
}
#endif
#endif
