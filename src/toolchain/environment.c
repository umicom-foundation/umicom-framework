/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/environment.c
 *
 * PURPOSE:
 *   Build deterministic child-process environment variables for CMake, Ninja,
 *   pkg-config, Clang, resource compilation, runtime DLL loading, and Java when
 *   available.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/environment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

void umi_environment_plan_init(UmiEnvironmentPlan *plan)
{
    if (plan != NULL) {
        (void)memset(plan, 0, sizeof(*plan));
    }
}

UmiStatus umi_environment_plan_add(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value)
{
    UmiEnvironmentPlanEntry *entry;
    if (plan == NULL || name == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->count >= UMI_ENVIRONMENT_PLAN_MAX ||
        strlen(name) >= sizeof(plan->entries[0].name) ||
        strlen(value) >= sizeof(plan->entries[0].value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &plan->entries[plan->count];
    (void)snprintf(entry->name, sizeof(entry->name), "%s", name);
    (void)snprintf(entry->value, sizeof(entry->value), "%s", value);
    plan->process_entries[plan->count].name = entry->name;
    plan->process_entries[plan->count].value = entry->value;
    plan->count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus umi_environment_add_tool(UmiEnvironmentPlan *plan,
                                          const char *name,
                                          const UmiToolchainProfile *profile,
                                          UmiToolKind kind)
{
    const UmiToolInfo *tool = umi_toolchain_profile_tool(profile, kind);
    if (tool == NULL || tool->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_environment_plan_add(plan, name, tool->path);
}

UmiStatus umi_environment_plan_from_toolchain(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan)
{
    const char *current_path;
    char path_value[UMI_ENVIRONMENT_VALUE_CAPACITY];
    const UmiToolInfo *java;
    UmiStatus status;
#ifdef _WIN32
    const char separator = ';';
#else
    const char separator = ':';
#endif

    if (profile == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_environment_plan_init(out_plan);

    current_path = getenv("PATH");
    if (profile->bin_directory[0] != '\0') {
        int written = snprintf(path_value,
                               sizeof(path_value),
                               "%s%c%s",
                               profile->bin_directory,
                               separator,
                               current_path != NULL ? current_path : "");
        if (written < 0 || (size_t)written >= sizeof(path_value)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_environment_plan_add(out_plan, "PATH", path_value);
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_environment_add_tool(out_plan,
                                      "CC",
                                      profile,
                                      UMI_TOOL_CLANG);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_environment_add_tool(out_plan,
                                   "CXX",
                                   profile,
                                   UMI_TOOL_CLANGXX);
    (void)umi_environment_add_tool(out_plan,
                                   "PKG_CONFIG",
                                   profile,
                                   UMI_TOOL_PKG_CONFIG);
    (void)umi_environment_add_tool(out_plan,
                                   "CMAKE_MAKE_PROGRAM",
                                   profile,
                                   UMI_TOOL_NINJA);
    (void)umi_environment_add_tool(out_plan,
                                   "RC",
                                   profile,
                                   UMI_TOOL_WINDRES);

    if (profile->prefix_directory[0] != '\0') {
        (void)umi_environment_plan_add(out_plan,
                                       "CMAKE_PREFIX_PATH",
                                       profile->prefix_directory);
    }

    java = umi_toolchain_profile_tool(profile, UMI_TOOL_JAVA);
    if (java != NULL && java->state == UMI_TOOL_VALIDATED) {
        char java_bin[UMI_PATH_CAPACITY];
        char java_home[UMI_PATH_CAPACITY];
        if (umi_fs_parent(java_bin, sizeof(java_bin), java->path) == UMI_STATUS_OK &&
            umi_fs_parent(java_home, sizeof(java_home), java_bin) == UMI_STATUS_OK) {
            (void)umi_environment_plan_add(out_plan, "JAVA_HOME", java_home);
        }
    }
    return UMI_STATUS_OK;
}

const UmiEnvironmentVariable *umi_environment_plan_variables(
    UmiEnvironmentPlan *plan)
{
    return plan != NULL ? plan->process_entries : NULL;
}

UmiStatus umi_environment_plan_write(const UmiEnvironmentPlan *plan,
                                     const char *path)
{
    char text[32768];
    size_t used = 0U;
    size_t index;
    int written;

    if (plan == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    text[0] = '\0';
    for (index = 0U; index < plan->count; ++index) {
        written = snprintf(text + used,
                           sizeof(text) - used,
                           "%s=%s\n",
                           plan->entries[index].name,
                           plan->entries[index].value);
        if (written < 0 || (size_t)written >= sizeof(text) - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return umi_fs_write_text(path, text);
}
