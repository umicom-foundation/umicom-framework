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

/**
 * Represent the developer project build plan data shared with callers of this public
 * contract.
 */
/* Commands contain self-relative pointer caches. Rebind each active command
 * after a plain value copy, or use the checked plan copy below. */
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

/**
 * Initialise developer project build plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_project_build_plan_init(
    UmiDeveloperProjectBuildPlan *plan,
    UmiDeveloperProjectBuildSystem build_system);

/**
 * Prepare configure/build, optional test and optional package commands without
 * invoking tools. A nonempty preset governs all phases; provide matching
 * configure/build/test preset names in the project. build_directory is metadata
 * when a preset is selected. Without a preset, the binary directory is required
 * and configure uses Ninja, with CMAKE_BUILD_TYPE when configuration is supplied.
 * Build/package use --config and CTest uses -C. Preset single-config generators
 * must already agree with the requested configuration in their cache settings.
 * Commands run from source_root; nonpreset configure uses -S . in that directory.
 *
 * All factories leave plan unchanged on refusal, including allocation failure.
 * Input strings may refer into the current plan and must be valid C strings.
 * Publication owns every command and rebinds its argv cache. No filesystem or
 * tool discovery occurs, and ordinary nonzero option flags retain their meaning.
 */
UmiStatus umi_developer_project_build_plan_cmake(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    const char *build_directory,
    const char *preset,
    const char *configuration,
    int include_test,
    int include_package);

/**
 * Provide the developer project build plan zig operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_build_plan_zig(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    int include_test);

/**
 * Provide the developer project build plan cargo operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_build_plan_cargo(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    int release,
    int include_test);

/**
 * Check that developer project build plan satisfies its contract before another service
 * relies on it.
 */
/* Also validate the four bounded metadata fields; unused command slots and
 * pointer caches are ignored. An initialized empty plan remains valid. */
UmiStatus umi_developer_project_build_plan_validate(
    const UmiDeveloperProjectBuildPlan *plan);

/* Validate owned metadata and active commands, copy and rebind argv. Self-copy
 * is supported; refusal leaves destination unchanged and allocates nothing. */
UmiStatus umi_developer_project_build_plan_copy(
    UmiDeveloperProjectBuildPlan *destination,
    const UmiDeveloperProjectBuildPlan *source);

#ifdef __cplusplus
}
#endif

#endif
