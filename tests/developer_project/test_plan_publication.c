/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_plan_publication.c
 *
 * PURPOSE:
 *   Inspect native project argv plans, refusal preservation, metadata admission and final-address ownership without executing tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/build_plan.h"
#include "umicom/developer_project/providers/cmake.h"
#include "umicom/developer_project/model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto cleanup; } } while (0)
static int Bound(const UmiDeveloperProjectBuildPlan *plan)
{
    for (size_t c = 0U; c < plan->command_count; ++c)
        for (size_t i = 0U; i < UMI_BUILD_MAX_ARGUMENTS; ++i)
            if (plan->commands[c].arguments[i] !=
                (i < plan->commands[c].argument_count ? plan->commands[c].argument_storage[i] : NULL))
                return 0;
    return 1;
}
static int Exact(const UmiBuildCommand *command, const char *program,
    const char *const *arguments, size_t count)
{
    if (strcmp(command->program, program) || strcmp(command->working_directory, "project with spaces") ||
        command->argument_count != count) return 0;
    for (size_t i = 0U; i < count; ++i)
        if (strcmp(command->arguments[i], arguments[i])) return 0;
    return 1;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *scenario = argv[1];
    const char *matrix[] = {
        "directory-default-build",
        "preset-default-build",
        "directory-debug-build",
        "preset-debug-build",
        "directory-default-build-test",
        "preset-default-build-test",
        "directory-debug-build-test",
        "preset-debug-build-test",
        "directory-default-build-package",
        "preset-default-build-package",
        "directory-debug-build-package",
        "preset-debug-build-package",
        "directory-default-build-test-package",
        "preset-default-build-test-package",
        "directory-debug-build-test-package",
        "preset-debug-build-test-package"
    };
    const char *other[] = {
        "cmake-alias",
        "zig-alias",
        "cargo-alias",
        "cmake-long-root",
        "zig-long-root",
        "cargo-long-root",
        "long-directory",
        "long-preset",
        "long-configuration",
        "null-output",
        "null-root",
        "missing-binary",
        "truthy-flags",
        "metadata-root",
        "metadata-directory",
        "metadata-preset",
        "metadata-configuration",
        "command-program",
        "argument-count",
        "invalid-enum",
        "excess-commands",
        "empty-initialized",
        "unused-command",
        "stale-cache",
        "copy-self",
        "copy-independent",
        "copy-invalid",
        "copy-null-source",
        "copy-null-destination",
        "provider-default",
        "provider-preset",
        "provider-null",
        "provider-malformed",
        "provider-null-output"
    };
    int mode = -1, known = 0;
    for (size_t i = 0U; i < sizeof matrix / sizeof matrix[0]; ++i)
        if (!strcmp(scenario, matrix[i])) { mode = (int)i; known = 1; }
    for (size_t i = 0U; i < sizeof other / sizeof other[0]; ++i)
        if (!strcmp(scenario, other[i])) known = 1;
    if (!known) return 2;
    int failed = 0;
    UmiDeveloperProjectBuildPlan *plan = malloc(sizeof *plan);
    UmiDeveloperProjectBuildPlan *saved = malloc(sizeof *saved);
    UmiDeveloperProjectBuildPlan *copy = malloc(sizeof *copy);
    char longRoot[UMI_DEVELOPER_PROJECT_PATH_CAPACITY + 1U];
    char longId[UMI_DEVELOPER_PROJECT_ID_CAPACITY + 1U];
    CHECK(plan && saved && copy);
    CHECK(umi_developer_project_build_plan_zig(plan, "previous", 1) == UMI_STATUS_OK);

    if (!strncmp(scenario, "provider-", 9U)) {
        UmiDeveloperProjectModel model;
        umi_developer_project_model_init(&model, "fixture", "Fixture");
        strcpy(model.root, "source"); strcpy(model.build_directory, "binary");
        strcpy(model.primary_language_id, "developer.language.c23");
        model.kind = UMI_DEVELOPER_PROJECT_EXECUTABLE;
        model.build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE;
        if (!strcmp(scenario, "provider-preset")) strcpy(model.preset, "release-preset");
        if (!strcmp(scenario, "provider-malformed")) memset(model.preset, 'x', sizeof model.preset);
        memcpy(saved, plan, sizeof *saved);
        const UmiDeveloperProjectLanguageProvider *provider = umi_developer_project_provider_cmake();
        UmiStatus result = provider->plan_build(!strcmp(scenario, "provider-null") ? NULL : &model,
            1, 1, !strcmp(scenario, "provider-null-output") ? NULL : plan);
        if (!strcmp(scenario, "provider-null") || !strcmp(scenario, "provider-malformed") ||
            !strcmp(scenario, "provider-null-output")) {
            CHECK(result == UMI_STATUS_INVALID_ARGUMENT && !memcmp(saved, plan, sizeof *saved));
        } else {
            CHECK(result == UMI_STATUS_OK && Bound(plan));
            if (!strcmp(scenario, "provider-preset")) {
                CHECK(plan->configuration[0] == '\0');
                CHECK(plan->commands[1].argument_count == 3U);
                CHECK(!strcmp(plan->commands[2].arguments[0], "--preset"));
                CHECK(!strcmp(plan->commands[2].arguments[1], "release-preset"));
            } else {
                CHECK(!strcmp(plan->configuration, "Debug"));
                CHECK(!strcmp(plan->commands[0].arguments[7], "CMAKE_BUILD_TYPE=Debug"));
            }
        }
        goto cleanup;
    }

    if (mode >= 0) {
        int preset = mode & 1, config = mode & 2, test = mode & 4, package = mode & 8;
        CHECK(umi_developer_project_build_plan_cmake(plan, "project with spaces", "binary folder",
            preset ? "selected" : NULL, config ? "Debug" : NULL, test, package) == UMI_STATUS_OK);
        CHECK(Bound(plan));
        CHECK(plan->command_count == 2U + (test ? 1U : 0U) + (package ? 1U : 0U));
        CHECK(!strcmp(plan->configuration, config ? "Debug" : ""));
        const char *configure[8]; size_t n = 0U;
        if (preset) { configure[n++] = "--preset"; configure[n++] = "selected"; }
        else {
            configure[n++] = "-S"; configure[n++] = "."; configure[n++] = "-B";
            configure[n++] = "binary folder"; configure[n++] = "-G"; configure[n++] = "Ninja";
            if (config) { configure[n++] = "-D"; configure[n++] = "CMAKE_BUILD_TYPE=Debug"; }
        }
        CHECK(Exact(&plan->commands[0], "cmake", configure, n));
        const char *build[7]; n = 0U; build[n++] = "--build";
        if (preset) build[n++] = "--preset";
        build[n++] = preset ? "selected" : "binary folder";
        if (config) { build[n++] = "--config"; build[n++] = "Debug"; }
        CHECK(Exact(&plan->commands[1], "cmake", build, n));
        size_t current = 2U;
        if (test) {
            const char *args[6]; size_t count = 0U;
            args[count++] = preset ? "--preset" : "--test-dir";
            args[count++] = preset ? "selected" : "binary folder";
            args[count++] = "--output-on-failure"; args[count++] = "--no-tests=error";
            if (config) { args[count++] = "-C"; args[count++] = "Debug"; }
            CHECK(Exact(&plan->commands[current++], "ctest", args, count));
        }
        if (package) {
            n = 0U; build[n++] = "--build";
            if (preset) build[n++] = "--preset";
            build[n++] = preset ? "selected" : "binary folder";
            build[n++] = "--target"; build[n++] = "package";
            if (config) { build[n++] = "--config"; build[n++] = "Debug"; }
            CHECK(Exact(&plan->commands[current], "cmake", build, n));
        }
        CHECK(umi_developer_project_build_plan_validate(plan) == UMI_STATUS_OK);
        goto cleanup;
    }
    if (strstr(scenario, "-alias")) {
        strcpy(plan->source_root, "owned source"); strcpy(plan->build_directory, "owned binary");
        strcpy(plan->preset, "owned-preset"); strcpy(plan->configuration, "Release");
        if (!strcmp(scenario, "cmake-alias"))
            CHECK(umi_developer_project_build_plan_cmake(plan, plan->source_root, plan->build_directory,
                plan->preset, plan->configuration, 1, 1) == UMI_STATUS_OK);
        else if (!strcmp(scenario, "zig-alias"))
            CHECK(umi_developer_project_build_plan_zig(plan, plan->source_root, 1) == UMI_STATUS_OK);
        else CHECK(umi_developer_project_build_plan_cargo(plan, plan->source_root, 1, 1) == UMI_STATUS_OK);
        CHECK(Bound(plan) && !strcmp(plan->source_root, "owned source"));
        CHECK(!strcmp(plan->commands[0].working_directory, "owned source"));
        goto cleanup;
    }
    if (strstr(scenario, "long-") || !strcmp(scenario, "null-output") ||
        !strcmp(scenario, "null-root") || !strcmp(scenario, "missing-binary")) {
        memset(longRoot, 'x', sizeof longRoot - 1U); longRoot[sizeof longRoot - 1U] = '\0';
        memset(longId, 'x', sizeof longId - 1U); longId[sizeof longId - 1U] = '\0';
        memcpy(saved, plan, sizeof *saved);
        UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED, result;
        if (!strcmp(scenario, "zig-long-root")) result = umi_developer_project_build_plan_zig(plan, longRoot, 1);
        else if (!strcmp(scenario, "cargo-long-root")) result = umi_developer_project_build_plan_cargo(plan, longRoot, 1, 1);
        else {
            const char *root = !strcmp(scenario, "cmake-long-root") ? longRoot : "source";
            const char *binary = !strcmp(scenario, "long-directory") ? longRoot : "binary";
            const char *preset = !strcmp(scenario, "long-preset") ? longId : NULL;
            const char *configuration = !strcmp(scenario, "long-configuration") ? longId : "Debug";
            if (!strcmp(scenario, "null-output") || !strcmp(scenario, "null-root") ||
                !strcmp(scenario, "missing-binary")) expected = UMI_STATUS_INVALID_ARGUMENT;
            if (!strcmp(scenario, "null-root")) root = NULL;
            if (!strcmp(scenario, "missing-binary")) binary = NULL;
            result = umi_developer_project_build_plan_cmake(!strcmp(scenario, "null-output") ? NULL : plan,
                root, binary, preset, configuration, 1, 1);
        }
        CHECK(result == expected);
        CHECK(!memcmp(saved, plan, sizeof *saved));
        goto cleanup;
    }
    if (!strcmp(scenario, "truthy-flags")) {
        CHECK(umi_developer_project_build_plan_cmake(plan, "source", "binary", NULL, NULL, -1, 7) == UMI_STATUS_OK);
        CHECK(plan->command_count == 4U);
        CHECK(umi_developer_project_build_plan_cargo(plan, "source", -3, 9) == UMI_STATUS_OK);
        CHECK(plan->command_count == 2U && !strcmp(plan->commands[0].arguments[1], "--release"));
        goto cleanup;
    }
    if (!strncmp(scenario, "copy-", 5U)) {
        memset(copy, 0x5a, sizeof *copy); memcpy(saved, copy, sizeof *saved);
        if (!strcmp(scenario, "copy-invalid")) memset(plan->preset, 'x', sizeof plan->preset);
        UmiDeveloperProjectBuildPlan *destination = !strcmp(scenario, "copy-self") ? plan :
            !strcmp(scenario, "copy-null-destination") ? NULL : copy;
        UmiStatus status = umi_developer_project_build_plan_copy(destination,
            !strcmp(scenario, "copy-null-source") ? NULL : plan);
        if (!strcmp(scenario, "copy-invalid") || !strcmp(scenario, "copy-null-source") ||
            !strcmp(scenario, "copy-null-destination")) {
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT && !memcmp(saved, copy, sizeof *copy));
        } else {
            CHECK(status == UMI_STATUS_OK && Bound(destination));
            if (!strcmp(scenario, "copy-independent")) memset(plan, 0, sizeof *plan);
            CHECK(!strcmp(destination->commands[1].arguments[1], "test"));
            CHECK(!strcmp(destination->source_root, "previous"));
        }
        goto cleanup;
    }
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (!strcmp(scenario, "metadata-root")) memset(plan->source_root, 'x', sizeof plan->source_root);
    else if (!strcmp(scenario, "metadata-directory")) memset(plan->build_directory, 'x', sizeof plan->build_directory);
    else if (!strcmp(scenario, "metadata-preset")) memset(plan->preset, 'x', sizeof plan->preset);
    else if (!strcmp(scenario, "metadata-configuration")) memset(plan->configuration, 'x', sizeof plan->configuration);
    else if (!strcmp(scenario, "command-program")) memset(plan->commands[1].program, 'x', sizeof plan->commands[1].program);
    else if (!strcmp(scenario, "argument-count")) { plan->commands[1].argument_count = SIZE_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(scenario, "invalid-enum")) plan->build_system = (UmiDeveloperProjectBuildSystem)99;
    else if (!strcmp(scenario, "excess-commands")) plan->command_count = SIZE_MAX;
    else if (!strcmp(scenario, "empty-initialized")) { umi_developer_project_build_plan_init(plan, UMI_DEVELOPER_PROJECT_BUILD_NONE); expected = UMI_STATUS_OK; }
    else if (!strcmp(scenario, "unused-command")) { memset(&plan->commands[2], 0x5a, sizeof plan->commands[2]); expected = UMI_STATUS_OK; }
    else if (!strcmp(scenario, "stale-cache")) {
        for (size_t c = 0U; c < plan->command_count; ++c)
            memset(plan->commands[c].arguments, 0, sizeof plan->commands[c].arguments);
        expected = UMI_STATUS_OK;
    }
    memcpy(saved, plan, sizeof *saved);
    CHECK(umi_developer_project_build_plan_validate(plan) == expected);
    CHECK(!memcmp(saved, plan, sizeof *saved));
cleanup:
    free(plan); free(saved); free(copy);
    return failed;
}
