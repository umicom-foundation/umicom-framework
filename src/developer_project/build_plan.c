/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/build_plan.c
 *
 * PURPOSE:
 *   Implement safe argv build plans for native and polyglot project systems.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/build_plan.h"

#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus add_command(
    UmiDeveloperProjectBuildPlan *plan,
    const char *program,
    const char *working_directory,
    const char *const *arguments,
    size_t argument_count)
{
    UmiBuildCommand *command;
    size_t index;

    if (plan == NULL || program == NULL ||
        plan->command_count >= UMI_DEVELOPER_PROJECT_BUILD_PLAN_COMMANDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    command = &plan->commands[plan->command_count];
    umi_build_command_init(command, program);

    if (working_directory != NULL && working_directory[0] != '\0' &&
        !umi_build_command_set_working_directory(
            command, working_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < argument_count; ++index) {
        if (!umi_build_command_add_argument(command, arguments[index])) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    plan->command_count += 1U;
    return UMI_STATUS_OK;
}

void umi_developer_project_build_plan_init(
    UmiDeveloperProjectBuildPlan *plan,
    UmiDeveloperProjectBuildSystem build_system)
{
    if (plan == NULL) return;

    (void)memset(plan, 0, sizeof(*plan));
    plan->build_system = build_system;
    plan->revision = 1U;
}

UmiStatus umi_developer_project_build_plan_cmake(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    const char *build_directory,
    const char *preset,
    const char *configuration,
    int include_test,
    int include_package)
{
    const char *configure_preset_args[2];
    const char *configure_dir_args[6];
    const char *build_preset_args[3];
    const char *build_dir_args[2];
    const char *test_args[5];
    const char *package_args[5];
    UmiStatus status;
    int use_preset;

    if (plan == NULL || source_root == NULL ||
        source_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    use_preset = preset != NULL && preset[0] != '\0';
    if (!use_preset &&
        (build_directory == NULL || build_directory[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_build_plan_init(
        plan, UMI_DEVELOPER_PROJECT_BUILD_CMAKE);

    status = copy_text(plan->source_root,
                       sizeof(plan->source_root),
                       source_root);
    if (status != UMI_STATUS_OK) return status;

    if (build_directory != NULL && build_directory[0] != '\0') {
        status = copy_text(plan->build_directory,
                           sizeof(plan->build_directory),
                           build_directory);
        if (status != UMI_STATUS_OK) return status;
    }

    if (use_preset) {
        status = copy_text(plan->preset, sizeof(plan->preset), preset);
        if (status != UMI_STATUS_OK) return status;
    }

    if (configuration != NULL && configuration[0] != '\0') {
        status = copy_text(
            plan->configuration,
            sizeof(plan->configuration),
            configuration);
        if (status != UMI_STATUS_OK) return status;
    }

    if (use_preset) {
        configure_preset_args[0] = "--preset";
        configure_preset_args[1] = preset;
        status = add_command(
            plan, "cmake", source_root,
            configure_preset_args, 2U);
    } else {
        configure_dir_args[0] = "-S";
        configure_dir_args[1] = source_root;
        configure_dir_args[2] = "-B";
        configure_dir_args[3] = build_directory;
        configure_dir_args[4] = "-G";
        configure_dir_args[5] = "Ninja";
        status = add_command(
            plan, "cmake", source_root,
            configure_dir_args, 6U);
    }
    if (status != UMI_STATUS_OK) return status;

    if (use_preset) {
        build_preset_args[0] = "--build";
        build_preset_args[1] = "--preset";
        build_preset_args[2] = preset;
        status = add_command(
            plan, "cmake", source_root,
            build_preset_args, 3U);
    } else {
        build_dir_args[0] = "--build";
        build_dir_args[1] = build_directory;
        status = add_command(
            plan, "cmake", source_root,
            build_dir_args, 2U);
    }
    if (status != UMI_STATUS_OK) return status;

    if (include_test) {
        size_t test_count = 0U;
        test_args[test_count++] = "--test-dir";
        test_args[test_count++] =
            build_directory != NULL && build_directory[0] != '\0'
                ? build_directory
                : "build";
        test_args[test_count++] = "--output-on-failure";

        if (configuration != NULL && configuration[0] != '\0') {
            test_args[test_count++] = "-C";
            test_args[test_count++] = configuration;
        }

        status = add_command(
            plan, "ctest", source_root, test_args, test_count);
        if (status != UMI_STATUS_OK) return status;
    }

    if (include_package) {
        size_t package_count = 0U;

        package_args[package_count++] = "--build";
        if (use_preset) {
            package_args[package_count++] = "--preset";
            package_args[package_count++] = preset;
        } else {
            package_args[package_count++] = build_directory;
        }
        package_args[package_count++] = "--target";
        package_args[package_count++] = "package";

        status = add_command(
            plan, "cmake", source_root,
            package_args, package_count);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_build_plan_zig(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    int include_test)
{
    const char *build_args[] = {"build"};
    const char *test_args[] = {"build", "test"};
    UmiStatus status;

    if (plan == NULL || source_root == NULL ||
        source_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_build_plan_init(
        plan, UMI_DEVELOPER_PROJECT_BUILD_ZIG);
    status = copy_text(
        plan->source_root, sizeof(plan->source_root), source_root);
    if (status != UMI_STATUS_OK) return status;

    status = add_command(
        plan, "zig", source_root, build_args, 1U);
    if (status != UMI_STATUS_OK) return status;

    if (include_test) {
        status = add_command(
            plan, "zig", source_root, test_args, 2U);
    }

    return status;
}

UmiStatus umi_developer_project_build_plan_cargo(
    UmiDeveloperProjectBuildPlan *plan,
    const char *source_root,
    int release,
    int include_test)
{
    const char *build_debug_args[] = {"build"};
    const char *build_release_args[] = {"build", "--release"};
    const char *test_debug_args[] = {"test"};
    const char *test_release_args[] = {"test", "--release"};
    UmiStatus status;

    if (plan == NULL || source_root == NULL ||
        source_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_build_plan_init(
        plan, UMI_DEVELOPER_PROJECT_BUILD_CARGO);
    status = copy_text(
        plan->source_root, sizeof(plan->source_root), source_root);
    if (status != UMI_STATUS_OK) return status;

    status = add_command(
        plan,
        "cargo",
        source_root,
        release ? build_release_args : build_debug_args,
        release ? 2U : 1U);
    if (status != UMI_STATUS_OK) return status;

    if (include_test) {
        status = add_command(
            plan,
            "cargo",
            source_root,
            release ? test_release_args : test_debug_args,
            release ? 2U : 1U);
    }

    return status;
}

UmiStatus umi_developer_project_build_plan_validate(
    const UmiDeveloperProjectBuildPlan *plan)
{
    size_t index;

    if (plan == NULL ||
        plan->build_system < UMI_DEVELOPER_PROJECT_BUILD_NONE ||
        plan->build_system > UMI_DEVELOPER_PROJECT_BUILD_CUSTOM ||
        plan->command_count > UMI_DEVELOPER_PROJECT_BUILD_PLAN_COMMANDS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < plan->command_count; ++index) {
        const UmiStatus status =
            umi_build_command_validate(&plan->commands[index], NULL, 0U);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
