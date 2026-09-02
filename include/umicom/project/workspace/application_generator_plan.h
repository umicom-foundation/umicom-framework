/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_generator_plan.h
 *
 * PURPOSE:
 *   Publish the public application generator plan contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATOR_PLAN_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATOR_PLAN_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace application generator plan step data shared with callers
 * of this public contract.
 */
    typedef struct UmiProjectWorkspaceApplicationGeneratorPlanStep {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char action[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        bool required;
        bool completed;
    }
    UmiProjectWorkspaceApplicationGeneratorPlanStep;
/**
 * Represent the project workspace application generator plan data shared with callers of
 * this public contract.
 */
    typedef struct UmiProjectWorkspaceApplicationGeneratorPlan {
        UmiProjectWorkspaceApplicationGeneratorPlanStep steps[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY];
        size_t count;
        size_t completed_count;
        uint64_t revision;
    }
    UmiProjectWorkspaceApplicationGeneratorPlan;
    void umi_project_workspace_application_generator_plan_init(UmiProjectWorkspaceApplicationGeneratorPlan *value);
    UmiStatus umi_project_workspace_application_generator_plan_add(UmiProjectWorkspaceApplicationGeneratorPlan *value,const char *id,const char *action,bool required);
    UmiStatus umi_project_workspace_application_generator_plan_complete(UmiProjectWorkspaceApplicationGeneratorPlan *value,const char *id);
    bool umi_project_workspace_application_generator_plan_ready(const UmiProjectWorkspaceApplicationGeneratorPlan *value);
#ifdef __cplusplus
}
#endif
#endif
