/* Umicom Framework | Workspace Import Plan | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_IMPORT_PLAN_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_IMPORT_PLAN_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceImportPlanStep {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char action[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        bool required;
        bool completed;
    }
    UmiProjectWorkspaceWorkspaceImportPlanStep;
    typedef struct UmiProjectWorkspaceWorkspaceImportPlan {
        UmiProjectWorkspaceWorkspaceImportPlanStep steps[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY];
        size_t count;
        size_t completed_count;
        uint64_t revision;
    }
    UmiProjectWorkspaceWorkspaceImportPlan;
    void umi_project_workspace_workspace_import_plan_init(UmiProjectWorkspaceWorkspaceImportPlan *value);
    UmiStatus umi_project_workspace_workspace_import_plan_add(UmiProjectWorkspaceWorkspaceImportPlan *value,const char *id,const char *action,bool required);
    UmiStatus umi_project_workspace_workspace_import_plan_complete(UmiProjectWorkspaceWorkspaceImportPlan *value,const char *id);
    bool umi_project_workspace_workspace_import_plan_ready(const UmiProjectWorkspaceWorkspaceImportPlan *value);
#ifdef __cplusplus
}
#endif
#endif
