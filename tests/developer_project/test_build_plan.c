/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_build_plan.c
 *
 * PURPOSE:
 *   Verify CMake, Zig and Cargo build plans use safe argv commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/build_plan.h"

int main(void)
{
    UmiDeveloperProjectBuildPlan plan;

    assert(umi_developer_project_build_plan_cmake(
        &plan,
        "C:/work/app",
        "build/windows-ucrt64-debug",
        "windows-ucrt64-debug",
        "Debug",
        1,
        1) == UMI_STATUS_OK);
    assert(plan.command_count == 4U);
    assert(strcmp(plan.commands[0].program, "cmake") == 0);
    assert(umi_developer_project_build_plan_validate(&plan) == UMI_STATUS_OK);

    assert(umi_developer_project_build_plan_zig(
        &plan, "C:/work/zig", 1) == UMI_STATUS_OK);
    assert(plan.command_count == 2U);

    assert(umi_developer_project_build_plan_cargo(
        &plan, "C:/work/rust", 0, 1) == UMI_STATUS_OK);
    assert(plan.command_count == 2U);
    return 0;
}
