/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_build.c
 *
 * PURPOSE:
 *   Implement native configure, build, test, run, make, repair, and prepared
 *   shell commands using the shared Framework build and toolchain services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *umi_cli_option_value(int argc,
                                        char **argv,
                                        const char *option)
{
    int index;
    for (index = 0; index + 1 < argc; ++index) {
        if (strcmp(argv[index], option) == 0) return argv[index + 1];
    }
    return NULL;
}

static int umi_cli_has_flag(int argc, char **argv, const char *option)
{
    int index;
    for (index = 0; index < argc; ++index) {
        if (strcmp(argv[index], option) == 0) return 1;
    }
    return 0;
}

int umi_cli_command_build(UmiCliContext *context,
                          UmiBuildAction action,
                          int argc,
                          char **argv)
{
    UmiBuildRequest request;
    UmiBuildReport report;
    const char *source_root = umi_cli_option_value(argc, argv, "--source");
    const char *build_override = umi_cli_option_value(argc, argv, "--build");
    const char *preset = umi_cli_option_value(argc, argv, "--preset");
    const char *target = umi_cli_option_value(argc, argv, "--target");
    const char *jobs_text = umi_cli_option_value(argc, argv, "--jobs");
    const char *executable = umi_cli_option_value(argc, argv, "--executable");
    const char *application_alias = argc > 0 && argv[0][0] != '-'
        ? argv[0]
        : NULL;
    char build_root[UMI_PATH_CAPACITY];
    char build_directory[UMI_PATH_CAPACITY];
    char derived_executable[UMI_PATH_CAPACITY];
    char executable_name[256];
    UmiStatus status;

    if (umi_cli_context_prepare(context, source_root, 0, 0) != UMI_STATUS_OK) {
        return 1;
    }
    if (preset == NULL && build_override == NULL) {
#ifdef _WIN32
        preset = "windows-ucrt64-headless-debug";
#else
        preset = "headless-debug";
#endif
    }
    if (build_override != NULL) {
        if (strlen(build_override) >= sizeof(build_directory)) return 1;
        (void)memcpy(build_directory,
                     build_override,
                     strlen(build_override) + 1U);
        preset = NULL;
    } else {
        status = umi_fs_join(build_root,
                             sizeof(build_root),
                             context->project_root,
                             "build");
        if (status != UMI_STATUS_OK) return 1;
        status = umi_fs_join(build_directory,
                             sizeof(build_directory),
                             build_root,
                             preset);
        if (status != UMI_STATUS_OK) return 1;
    }

    derived_executable[0] = '\0';
    if (action == UMI_BUILD_RUN && executable == NULL &&
        application_alias != NULL) {
        const char *base_name = application_alias;
        if (strcmp(application_alias, "studio") == 0 ||
            strcmp(application_alias, "ide") == 0) {
            base_name = "umicom-studio-ide";
        } else if (strcmp(application_alias, "console") == 0) {
            base_name = "umicom-studio-console";
        } else if (strcmp(application_alias, "doctor") == 0) {
            base_name = "umicom-studio-doctor";
        } else if (strcmp(application_alias, "diagnostics") == 0) {
            base_name = "umicom-studio-diagnostics";
        } else if (strcmp(application_alias, "settings") == 0) {
            base_name = "umicom-studio-settings";
        } else if (strcmp(application_alias, "platform") == 0) {
            base_name = "umicom-studio-platform";
        }
#ifdef _WIN32
        if (snprintf(executable_name,
                     sizeof(executable_name),
                     "%s.exe",
                     base_name) < 0) return 1;
#else
        if (strlen(base_name) >= sizeof(executable_name)) return 1;
        (void)memcpy(executable_name, base_name, strlen(base_name) + 1U);
#endif
        status = umi_fs_join(build_root,
                             sizeof(build_root),
                             build_directory,
                             "bin");
        if (status != UMI_STATUS_OK) return 1;
        status = umi_fs_join(derived_executable,
                             sizeof(derived_executable),
                             build_root,
                             executable_name);
        if (status != UMI_STATUS_OK) return 1;
        executable = derived_executable;
    }
    (void)memset(&request, 0, sizeof(request));
    request.source_root = context->project_root;
    request.build_directory = build_directory;
    request.preset = preset;
    request.target = target;
    request.executable = executable;
    request.jobs = jobs_text != NULL ? atoi(jobs_text) : 0;
    request.clean = umi_cli_has_flag(argc, argv, "--clean");

    if (action == UMI_BUILD_RUN && executable == NULL) {
        (void)fprintf(stderr,
                      "run requires --executable PATH\n");
        return 2;
    }
    status = umi_build_execute(&context->discovery.profile,
                               &context->environment,
                               action,
                               &request,
                               &report);
    if (report.last_output[0] != '\0') {
        (void)fputs(report.last_output, stdout);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Umicom build action failed: %s\n",
                      umi_status_text(status));
        return 1;
    }
    return 0;
}

int umi_cli_command_repair(UmiCliContext *context, int argc, char **argv)
{
    char preset_path[UMI_PATH_CAPACITY];
    char build_directory[UMI_PATH_CAPACITY];
    char recovery_path[UMI_PATH_CAPACITY];
    int dry_run = umi_cli_has_flag(argc, argv, "--dry-run");
    UmiStatus status;

    if (umi_cli_context_prepare(context, NULL, 0, 0) != UMI_STATUS_OK) {
        return 1;
    }
    (void)umi_fs_join(preset_path,
                      sizeof(preset_path),
                      context->project_root,
                      "CMakeUserPresets.json");
    (void)umi_fs_join(build_directory,
                      sizeof(build_directory),
                      context->project_root,
                      "build/umicom-auto-debug");
    status = umi_build_repair_cache(&context->discovery.profile,
                                    build_directory,
                                    recovery_path,
                                    sizeof(recovery_path),
                                    dry_run);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Cache repair failed: %s\n",
                      umi_status_text(status));
        return 1;
    }
    if (recovery_path[0] != '\0') {
        (void)printf("Stale cache recovery path: %s\n", recovery_path);
    }
    if (!dry_run) {
        status = umi_build_write_user_presets(&context->discovery.profile,
                                              context->project_root,
                                              preset_path);
        if (status != UMI_STATUS_OK) {
            (void)fprintf(stderr,
                          "User preset generation failed: %s\n",
                          umi_status_text(status));
            return 1;
        }
        (void)printf("Generated: %s\n", preset_path);
    } else {
        (void)printf("Dry run: would generate %s\n", preset_path);
    }
    return 0;
}

int umi_cli_command_shell(UmiCliContext *context, int argc, char **argv)
{
    const char *shell = argc > 0 ? argv[0] : NULL;
    if (umi_cli_context_prepare(context, NULL, 0, 0) != UMI_STATUS_OK) {
        return 1;
    }
    return umi_build_open_shell(&context->discovery.profile,
                                &context->environment,
                                context->project_root,
                                shell) == UMI_STATUS_OK ? 0 : 1;
}
