/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/repository_bootstrap_plan.h
 *
 * PURPOSE:
 *   Publish the public repository bootstrap plan contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_REPOSITORY_BOOTSTRAP_PLAN_H
#define UMICOM_PROJECT_WORKSPACE_REPOSITORY_BOOTSTRAP_PLAN_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace repository bootstrap plan step data shared with callers
 * of this public contract.
 */
    typedef struct UmiProjectWorkspaceRepositoryBootstrapPlanStep {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char action[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        bool required;
        bool completed;
    }
    UmiProjectWorkspaceRepositoryBootstrapPlanStep;
/**
 * Represent the project workspace repository bootstrap plan data shared with callers of
 * this public contract.
 */
    typedef struct UmiProjectWorkspaceRepositoryBootstrapPlan {
        UmiProjectWorkspaceRepositoryBootstrapPlanStep steps[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY];
        size_t count;
        size_t completed_count;
        uint64_t revision;
    }
    UmiProjectWorkspaceRepositoryBootstrapPlan;
    void umi_project_workspace_repository_bootstrap_plan_init(UmiProjectWorkspaceRepositoryBootstrapPlan *value);
    UmiStatus umi_project_workspace_repository_bootstrap_plan_add(UmiProjectWorkspaceRepositoryBootstrapPlan *value,const char *id,const char *action,bool required);
    UmiStatus umi_project_workspace_repository_bootstrap_plan_complete(UmiProjectWorkspaceRepositoryBootstrapPlan *value,const char *id);
    bool umi_project_workspace_repository_bootstrap_plan_ready(const UmiProjectWorkspaceRepositoryBootstrapPlan *value);
#ifdef __cplusplus
}
#endif
#endif
