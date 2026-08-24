/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/scoped_environment.c
 *
 * PURPOSE:
 *   Implement permissive operation-scoped environments that do not require CC for Git operations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/scoped_environment.h"

#include <stdlib.h>

static void add_if_valid(UmiEnvironmentPlan *plan, const char *name,
                         const UmiToolchainProfile *profile, UmiToolKind kind)
{
    const UmiToolInfo *tool = umi_toolchain_profile_tool(profile, kind);
    if (tool != NULL && tool->state == UMI_TOOL_VALIDATED) {
        (void)umi_environment_plan_set(plan, name, tool->path);
    }
}

UmiStatus umi_toolchain_scoped_environment(
    const UmiToolchainProfile *profile,
    const UmiToolchainOperationProfile *operation,
    UmiEnvironmentPlan *out_plan)
{
    const char *path;
    UmiStatus status;
    if (profile == NULL || operation == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_environment_plan_init(out_plan);
    path = getenv("PATH");
    if (operation->inherit_path && path != NULL && path[0] != '\0') {
        status = umi_environment_plan_set(out_plan, "PATH", path);
        if (status != UMI_STATUS_OK) return status;
    }
    if (profile->bin_directory[0] != '\0') {
        if (umi_environment_plan_find(out_plan, "PATH") == NULL) {
            status = umi_environment_plan_set(out_plan, "PATH", profile->bin_directory);
        } else {
            status = umi_environment_plan_append_path(out_plan, profile->bin_directory, 1);
        }
        if (status != UMI_STATUS_OK) return status;
    }

    add_if_valid(out_plan, "CC", profile, profile->selected_c_compiler);
    add_if_valid(out_plan, "CXX", profile, profile->selected_cpp_compiler);
    add_if_valid(out_plan, "CMAKE_MAKE_PROGRAM", profile, UMI_TOOL_NINJA);
    add_if_valid(out_plan, "PKG_CONFIG", profile, UMI_TOOL_PKG_CONFIG);
    add_if_valid(out_plan, "RC", profile, UMI_TOOL_WINDRES);
    return UMI_STATUS_OK;
}
