/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/build_plan.h
 *
 * PURPOSE:
 *   Plan safe argv-based configure/build/test/package commands for project
 *   build systems without invoking a shell or embedding commands in Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_BUILD_PLAN_H
#define UMICOM_DEVELOPER_PROJECT_BUILD_PLAN_H

#include "umicom/build/types.h"
#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_BUILD_PLAN_COMMANDS 8U

typedef struct UmiDeveloperProjectBuildPlan {
    UmiDeveloperProjectBuildSystem build_system;
    UmiBuildCommand commands[UMI_DEVELOPER_PROJECT_BUILD_PLAN_COMMANDS];
    size_t command_count;
    char source_root[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char build_directory[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char preset[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char configuration[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperProjectBuildPlan;

void umi_developer_project_build_plan_init(
    UmiDeveloperProjectBuildPlan *plan,
    UmiDeveloperProjectBuildSystem build_system);

UmiStatus umi_developer_project_build_plan_cmake(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    const char *build_directory,
    const char *preset,
    const char *configuration,
    int include_test,
    int include_package);

UmiStatus umi_developer_project_build_plan_zig(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    int include_test);

UmiStatus umi_developer_project_build_plan_cargo(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    int release,
    int include_test);

UmiStatus umi_developer_project_build_plan_validate(
    const UmiDeveloperProjectBuildPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
