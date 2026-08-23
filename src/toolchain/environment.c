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

#include <ctype.h>
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

static void umi_environment_plan_rebind(UmiEnvironmentPlan *plan)
{
    size_t index;
    if (plan == NULL) return;
    for (index = 0U; index < plan->count; ++index) {
        plan->process_entries[index].name = plan->entries[index].name;
        plan->process_entries[index].value = plan->entries[index].value;
    }
}

const char *umi_environment_plan_find(const UmiEnvironmentPlan *plan,
                                      const char *name)
{
    size_t index;

    if (plan == NULL || name == NULL) return NULL;

    for (index = 0U; index < plan->count; ++index) {
        if (strcmp(plan->entries[index].name, name) == 0) {
            return plan->entries[index].value;
        }
    }

    return NULL;
}

UmiStatus umi_environment_plan_set(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value)
{
    size_t index;

    if (plan == NULL || name == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(name) >= sizeof(plan->entries[0].name) ||
        strlen(value) >= sizeof(plan->entries[0].value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < plan->count; ++index) {
        if (strcmp(plan->entries[index].name, name) != 0) continue;

        (void)snprintf(plan->entries[index].value,
                       sizeof(plan->entries[index].value),
                       "%s",
                       value);
        umi_environment_plan_rebind(plan);
        return UMI_STATUS_OK;
    }

    return umi_environment_plan_add(plan, name, value);
}

UmiStatus umi_environment_plan_validate(const UmiEnvironmentPlan *plan)
{
    size_t index;
    size_t other;

    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->count > UMI_ENVIRONMENT_PLAN_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < plan->count; ++index) {
        const char *name = plan->entries[index].name;
        const char *value = plan->entries[index].value;

        if (memchr(name, '\0', sizeof(plan->entries[index].name)) == NULL ||
            memchr(value, '\0', sizeof(plan->entries[index].value)) == NULL ||
            name[0] == '\0' || strchr(name, '=') != NULL ||
            strchr(name, '\n') != NULL || strchr(name, '\r') != NULL ||
            strchr(value, '\n') != NULL || strchr(value, '\r') != NULL) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        for (other = index + 1U; other < plan->count; ++other) {
            if (strcmp(name, plan->entries[other].name) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_environment_plan_compose(const UmiEnvironmentPlan *base,
                                       const UmiEnvironmentPlan *overlay,
                                       UmiEnvironmentPlan *out_plan)
{
    size_t index;
    UmiStatus status;

    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_environment_plan_init(out_plan);

    if (base != NULL) {
        status = umi_environment_plan_validate(base);
        if (status != UMI_STATUS_OK) return status;

        for (index = 0U; index < base->count; ++index) {
            status = umi_environment_plan_add(
                out_plan,
                base->entries[index].name,
                base->entries[index].value);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    if (overlay != NULL) {
        status = umi_environment_plan_validate(overlay);
        if (status != UMI_STATUS_OK) return status;

        for (index = 0U; index < overlay->count; ++index) {
            status = umi_environment_plan_set(
                out_plan,
                overlay->entries[index].name,
                overlay->entries[index].value);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_environment_plan_append_path(UmiEnvironmentPlan *plan,
                                           const char *directory,
                                           int prepend)
{
    const char *current;
    char composed[UMI_ENVIRONMENT_VALUE_CAPACITY];
    int written;
#ifdef _WIN32
    const char separator = ';';
#else
    const char separator = ':';
#endif

    if (plan == NULL || directory == NULL || directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    current = umi_environment_plan_find(plan, "PATH");
    if (current == NULL || current[0] == '\0') {
        return umi_environment_plan_set(plan, "PATH", directory);
    }

    written = prepend
        ? snprintf(composed, sizeof(composed), "%s%c%s",
                   directory, separator, current)
        : snprintf(composed, sizeof(composed), "%s%c%s",
                   current, separator, directory);

    if (written < 0 || (size_t)written >= sizeof(composed)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return umi_environment_plan_set(plan, "PATH", composed);
}

static int portable_environment_name(const char *name)
{
    size_t index;

    if (name == NULL || name[0] == '\0') return 0;
    if (!(isalpha((unsigned char)name[0]) || name[0] == '_')) return 0;

    for (index = 1U; name[index] != '\0'; ++index) {
        const unsigned char value = (unsigned char)name[index];
        if (!(isalnum(value) || value == '_')) return 0;
    }

    return 1;
}

UmiStatus umi_environment_assignment_parse(
    const char *assignment,
    char *out_name,
    size_t name_capacity,
    char *out_value,
    size_t value_capacity)
{
    const char *equals;
    size_t name_length;
    size_t value_length;

    if (assignment == NULL || out_name == NULL || name_capacity == 0U ||
        out_value == NULL || value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    equals = strchr(assignment, '=');
    if (equals == NULL || equals == assignment) {
        return UMI_STATUS_PARSE_ERROR;
    }

    name_length = (size_t)(equals - assignment);
    value_length = strlen(equals + 1U);

    if (name_length + 1U > name_capacity ||
        value_length + 1U > value_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(out_name, assignment, name_length);
    out_name[name_length] = '\0';
    (void)memcpy(out_value, equals + 1U, value_length + 1U);

    if (!portable_environment_name(out_name) ||
        strchr(out_value, '\n') != NULL ||
        strchr(out_value, '\r') != NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_environment_plan_set_assignment(
    UmiEnvironmentPlan *plan,
    const char *assignment)
{
    char name[UMI_ENVIRONMENT_NAME_CAPACITY];
    char value[UMI_ENVIRONMENT_VALUE_CAPACITY];
    UmiStatus status;

    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_environment_assignment_parse(
        assignment,
        name,
        sizeof(name),
        value,
        sizeof(value));
    if (status != UMI_STATUS_OK) return status;

    return umi_environment_plan_set(plan, name, value);
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

    status = umi_environment_add_tool(
        out_plan, "CC", profile, profile->selected_c_compiler);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_environment_add_tool(
        out_plan, "CXX", profile, profile->selected_cpp_compiler);
    (void)umi_environment_add_tool(
        out_plan, "PKG_CONFIG", profile, UMI_TOOL_PKG_CONFIG);
    (void)umi_environment_add_tool(
        out_plan, "CMAKE_MAKE_PROGRAM", profile, UMI_TOOL_NINJA);
    (void)umi_environment_add_tool(
        out_plan, "RC", profile, UMI_TOOL_WINDRES);

    if (profile->prefix_directory[0] != '\0') {
        (void)umi_environment_plan_add(
            out_plan, "CMAKE_PREFIX_PATH", profile->prefix_directory);
    }

    java = umi_toolchain_profile_tool(profile, UMI_TOOL_JAVA);
    if (java != NULL && java->state == UMI_TOOL_VALIDATED) {
        char java_bin[UMI_PATH_CAPACITY];
        char java_home[UMI_PATH_CAPACITY];

        if (umi_fs_parent(java_bin, sizeof(java_bin), java->path) ==
                UMI_STATUS_OK &&
            umi_fs_parent(java_home, sizeof(java_home), java_bin) ==
                UMI_STATUS_OK) {
            (void)umi_environment_plan_add(
                out_plan, "JAVA_HOME", java_home);
        }
    }

    return UMI_STATUS_OK;
}

const UmiEnvironmentVariable *umi_environment_plan_variables(
    UmiEnvironmentPlan *plan)
{
    if (plan == NULL) return NULL;
    umi_environment_plan_rebind(plan);
    return plan->process_entries;
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

        if (written < 0 ||
            (size_t)written >= sizeof(text) - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return umi_fs_write_text(path, text);
}
