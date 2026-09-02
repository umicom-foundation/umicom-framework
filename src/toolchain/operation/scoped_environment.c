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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/scoped_environment.h"

#include <stdlib.h>

/* Check that add if satisfies its contract before another service relies on it. */
static void add_if_valid(UmiEnvironmentPlan *plan, const char *name,
                         const UmiToolchainProfile *profile, UmiToolKind kind)
{
    const UmiToolInfo *tool = umi_toolchain_profile_tool(profile, kind);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tool != NULL && tool->state == UMI_TOOL_VALIDATED) {
        (void)umi_environment_plan_set(plan, name, tool->path);
    }
}

/*
 * Provide the toolchain scoped environment operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_scoped_environment(
    const UmiToolchainProfile *profile,
    const UmiToolchainOperationProfile *operation,
    UmiEnvironmentPlan *out_plan)
{
    const char *path;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || operation == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_environment_plan_init(out_plan);
    path = getenv("PATH");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation->inherit_path && path != NULL && path[0] != '\0') {
        status = umi_environment_plan_set(out_plan, "PATH", path);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->bin_directory[0] != '\0') {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (umi_environment_plan_find(out_plan, "PATH") == NULL) {
            status = umi_environment_plan_set(out_plan, "PATH", profile->bin_directory);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = umi_environment_plan_append_path(out_plan, profile->bin_directory, 1);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    add_if_valid(out_plan, "CC", profile, profile->selected_c_compiler);
    add_if_valid(out_plan, "CXX", profile, profile->selected_cpp_compiler);
    add_if_valid(out_plan, "CMAKE_MAKE_PROGRAM", profile, UMI_TOOL_NINJA);
    add_if_valid(out_plan, "PKG_CONFIG", profile, UMI_TOOL_PKG_CONFIG);
    add_if_valid(out_plan, "RC", profile, UMI_TOOL_WINDRES);
    return UMI_STATUS_OK;
}
