/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_build.c
 *
 * PURPOSE:
 *   Implement native configure, build, test, run, make, repair, and prepared
 *   shell commands using the shared Framework build and toolchain services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"
#include "umicom/toolchain/build_invocation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the cli option value operation used by this module and its client applications. */
/* Framework invocation parsing replaces this permissive lookup. Preserve the
 * former helper disabled so it cannot create an unused-function warning. */
#if 0
static const char *umi_cli_option_value(int argc,
                                        char **argv,
                                        const char *option)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index + 1 < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) return argv[index + 1];
    }
    return NULL;
}

#endif

/* Provide the cli has flag operation used by this module and its client applications. */
static int umi_cli_has_flag(int argc, char **argv, const char *option)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], option) == 0) return 1;
    }
    return 0;
}

/*
 * Provide the cli build report exit code operation used by this module and its client
 * applications.
 */
static int umi_cli_build_report_exit_code(const UmiBuildReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return -1;
    /* Select the behaviour associated with the requested command or state value. */
    switch (report->last_action) {
        case UMI_BUILD_CONFIGURE: return report->configure_exit_code;
        case UMI_BUILD_COMPILE: return report->build_exit_code;
        case UMI_BUILD_TEST: return report->test_exit_code;
        case UMI_BUILD_RUN: return report->run_exit_code;
        case UMI_BUILD_INSTALL: return report->install_exit_code;
        case UMI_BUILD_PACKAGE: return report->package_exit_code;
        default: return -1;
    }
}

/* Provide the cli command build operation used by this module and its client applications. */
/* Argument validation, path ownership and execution now live in Framework; the CLI only presents the selected commands and streamed output. The earlier implementation is retained for engineering review. */
#if 0
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
    char application_executable[256];
    UmiStatus status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_cli_context_prepare(context, source_root, 0, 0) != UMI_STATUS_OK) {
        return 1;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preset == NULL && build_override == NULL) {
#ifdef _WIN32
        preset = "windows-ucrt64-headless-debug";
#else
        preset = "headless-debug";
#endif
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (build_override != NULL) {
        /* Create this optional product surface only when its build option is enabled. */
        if (strlen(build_override) >= sizeof(build_directory)) return 1;
        (void)memcpy(build_directory,
                     build_override,
                     strlen(build_override) + 1U);
        preset = NULL;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_fs_join(build_root,
                             sizeof(build_root),
                             context->project_root,
                             "build");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return 1;
        status = umi_fs_join(build_directory,
                             sizeof(build_directory),
                             build_root,
                             preset);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return 1;
    }

    derived_executable[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (action == UMI_BUILD_RUN && executable == NULL &&
        application_alias != NULL) {
        const char *base_name = application_alias;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(application_alias, "studio") == 0 ||
            strcmp(application_alias, "ide") == 0) {
            base_name = "umicom-studio-ide";
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(application_alias, "console") == 0) {
            base_name = "umicom-studio-console";
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(application_alias, "doctor") == 0) {
            base_name = "umicom-studio-doctor";
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(application_alias, "diagnostics") == 0) {
            base_name = "umicom-studio-diagnostics";
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(application_alias, "settings") == 0) {
            base_name = "umicom-studio-settings";
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(application_alias, "platform") == 0) {
            base_name = "umicom-studio-platform";
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(application_alias, "desk") == 0 ||
                   strcmp(application_alias, "desktop") == 0) {
            base_name = "umicom-desk";
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(application_alias, "umicom-", 7U) != 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (snprintf(application_executable,
                         sizeof(application_executable),
                         "umicom-%s",
                         application_alias) < 0) return 1;
            base_name = application_executable;
        }
#ifdef _WIN32
        /* Apply this branch only when its contract condition is satisfied. */
        if (snprintf(executable_name,
                     sizeof(executable_name),
                     "%s.exe",
                     base_name) < 0) return 1;
#else
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strlen(base_name) >= sizeof(executable_name)) return 1;
        (void)memcpy(executable_name, base_name, strlen(base_name) + 1U);
#endif
        status = umi_fs_join(build_root,
                             sizeof(build_root),
                             build_directory,
                             "bin");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return 1;
        status = umi_fs_join(derived_executable,
                             sizeof(derived_executable),
                             build_root,
                             executable_name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return 1;
        executable = derived_executable;
    }
    umi_build_request_init(&request);
    request.source_root = context->project_root;
    request.build_directory = build_directory;
    request.preset = preset;
    request.target = target;
    request.executable = executable;
    request.jobs = jobs_text != NULL ? atoi(jobs_text) : 0;
    request.clean = umi_cli_has_flag(argc, argv, "--clean");
    request.configuration = umi_cli_option_value(argc, argv, "--config");
    request.install_prefix = umi_cli_option_value(argc, argv, "--prefix");
    request.test_expression = umi_cli_option_value(argc, argv, "--tests");
    request.install_component = umi_cli_option_value(argc, argv, "--component");
    request.package_target = umi_cli_option_value(
        argc, argv, "--package-target");

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (report.output_truncated) {
        (void)fputs(
            "[umicom] Earlier child-process output was omitted; "
            "the final diagnostic output follows.\n",
            stderr);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (report.last_output[0] != '\0') {
        (void)fputs(report.last_output, stdout);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        int child_exit_code = umi_cli_build_report_exit_code(&report);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (child_exit_code >= 0) {
            (void)fprintf(
                stderr,
                "Umicom %s action failed (child exit code %d): %s\n",
                umi_build_action_text(report.last_action),
                child_exit_code,
                umi_status_text(status));
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)fprintf(stderr,
                          "Umicom %s action failed: %s\n",
                          umi_build_action_text(report.last_action),
                          umi_status_text(status));
        }
        return 1;
    }
    return 0;
}
#endif
/* Stream raw bytes while the child is running; do not print the bounded tail
 * again afterwards. The file sink independently retains the complete stream. */
static void build_output(const char *bytes, size_t size, void *context)
{
    (void)context;
    (void)fwrite(bytes, 1U, size, stdout);
    (void)fflush(stdout);
}

/* Quote for human inspection, not for shell execution. Each printed argument
 * is passed as one literal value through the Framework process service. */
static void build_print_argument(const char *text)
{
    (void)fputc('"', stdout);
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p == '"' || *p == '\\') (void)fputc('\\', stdout);
        if (*p < 32U) (void)printf("\\x%02x", (unsigned)*p);
        else (void)fputc(*p, stdout);
    }
    (void)fputc('"', stdout);
}

static void build_command(UmiBuildAction action, const UmiProcessRequest *command, void *context)
{
    (void)context;
    (void)printf("\n[%s] directory: %s\n", umi_build_action_text(action), command->working_directory);
    build_print_argument(command->program);
    for (size_t i = 0U; i < command->argument_count; ++i) {
        (void)fputc(' ', stdout);
        build_print_argument(command->arguments[i]);
    }
    (void)fputc('\n', stdout);
    (void)fflush(stdout);
}

int umi_cli_command_build(UmiCliContext *context, UmiBuildAction action, int argc, char **argv)
{
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) {
        (void)puts("Incremental: umicom build-test --build PATH --target NAME --tests REGEX [options]\n"
            "Build-test compiles, then tests; it does not request configure, clean or install.\n"
            "The target must build every executable/fixture needed by the selected tests.\n"
            "Use an existing build tree; --preset and --define are not accepted.\n");
        (void)puts("Usage: umicom configure|build|test|install|package|make|deliver|run|launch [options]\n"
            "  --source PATH      Project root (default: current directory)\n"
            "  --preset NAME      CMake configure/build/test preset\n"
            "  --build PATH       Explicit binary directory; overrides preset binaryDir\n"
            "  --config NAME      Build/test/install/package configuration\n"
            "  --define NAME=VALUE  Configure cache definition (repeat, up to 16)\n"
            "  --jobs N           Build/test/package parallel jobs (1..1024)\n"
            "  --target NAME      Build target; --tests REGEX selects tests\n"
            "  --prefix PATH      Installation destination; --component NAME is optional\n"
            "  --package-target NAME  Configured package target (default: package)\n"
            "  --executable PATH  Launch path; or use run studio --build PATH\n"
            "  --timeout-ms N     Per-process timeout (1..86400000)\n"
            "  --log PATH         New output file; parent must exist, never overwritten\n"
            "  --dry-run          Show commands without discovery, execution or log creation\n"
            "  --clean            Explicit configure/make/deliver cache cleanup\n"
            "  -- ARGUMENTS       Literal launch arguments only\n"
            "Install/deliver require --build and --prefix. Launch aliases need --build.\n"
            "Make: configure, build, test. Deliver: make, install, package. Stop on failure.\n"
            "Paths are relative to --source. Logs may contain private source paths and output.");
        return 0;
    }
    if (!context) return 2;
    UmiBuildInvocation *inv = calloc(1U, sizeof *inv);
    UmiBuildReport *report = calloc(1U, sizeof *report);
    if (!inv || !report) { free(inv); free(report); return 1; }
    char cwd[UMI_PATH_CAPACITY], message[256] = {0};
    UmiStatus status = umi_fs_current_directory(cwd, sizeof cwd);
    if (status == UMI_STATUS_OK) status = UmiBuildInvocationParse(action, argc,
        (const char *const *)argv, cwd, inv, message, sizeof message);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "%s: %s\n", umi_status_text(status), message);
        free(inv); free(report); return 2;
    }
    if (inv->options.preview) {
        /* A preview must work even before tools are installed. The native
         * executor uses descriptive tool names and never launches a process. */
        memset(context, 0, sizeof *context);
        umi_toolchain_profile_init(&context->discovery.profile);
        (void)puts("Preview only; no tools, build actions, logs or Git operations are executed.");
    } else if (action == UMI_BUILD_RUN) {
        status = umi_cli_context_prepare_operation(context, inv->source, UMI_TOOLCHAIN_OPERATION_RUN);
    } else {
        status = umi_cli_context_prepare(context, inv->source, 0, 0);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot prepare tools: %s\n", umi_status_text(status));
        free(inv); free(report); return 1;
    }
    if (inv->options.preview && (action == UMI_BUILD_CLEAN || inv->request.clean))
        (void)printf("Preview: clean only the selected project binary directory: %s\n", inv->binary);
    inv->options.output = build_output;
    inv->options.command = build_command;
    UmiBuildExecutionLog log;
    status = UmiBuildExecuteLogged(&context->discovery.profile, &context->environment,
        action, &inv->request, &inv->options, inv->log[0] ? inv->log : NULL, &log, report);
    if (log.created) (void)printf("\nLog: %s (%llu bytes; %s)\n", log.file.path,
        (unsigned long long)log.file.bytes_written, log.capture_complete ? "capture closed" : "capture incomplete");
    if (log.log_status != UMI_STATUS_OK)
        (void)fprintf(stderr, "Log operation failed: %s\n", umi_status_text(log.log_status));
    if (status != UMI_STATUS_OK) (void)fprintf(stderr, "%s failed: %s (child exit %d). Later phases were not run.\n",
        umi_build_action_text(report->last_action), umi_status_text(status), umi_cli_build_report_exit_code(report));
    else if (inv->options.preview) (void)puts("Preview complete; this is not build or test evidence.");
    else (void)printf("%s completed.\n", umi_build_action_text(action));
    free(inv); free(report);
    return status == UMI_STATUS_OK ? 0 : 1;
}

/*
 * Provide the cli command repair operation used by this module and its client
 * applications.
 */
int umi_cli_command_repair(UmiCliContext *context, int argc, char **argv)
{
    char preset_path[UMI_PATH_CAPACITY];
    char build_directory[UMI_PATH_CAPACITY];
    char recovery_path[UMI_PATH_CAPACITY];
    int dry_run = umi_cli_has_flag(argc, argv, "--dry-run");
    UmiStatus status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Cache repair failed: %s\n",
                      umi_status_text(status));
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (recovery_path[0] != '\0') {
        (void)printf("Stale cache recovery path: %s\n", recovery_path);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!dry_run) {
        status = umi_build_write_user_presets(&context->discovery.profile,
                                              context->project_root,
                                              preset_path);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)fprintf(stderr,
                          "User preset generation failed: %s\n",
                          umi_status_text(status));
            return 1;
        }
        (void)printf("Generated: %s\n", preset_path);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)printf("Dry run: would generate %s\n", preset_path);
    }
    return 0;
}

/* Provide the cli command shell operation used by this module and its client applications. */
int umi_cli_command_shell(UmiCliContext *context, int argc, char **argv)
{
    const char *shell = argc > 0 ? argv[0] : NULL;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_cli_context_prepare(context, NULL, 0, 0) != UMI_STATUS_OK) {
        return 1;
    }
    return umi_build_open_shell(&context->discovery.profile,
                                &context->environment,
                                context->project_root,
                                shell) == UMI_STATUS_OK ? 0 : 1;
}
