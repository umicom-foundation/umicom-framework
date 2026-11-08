/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_check.c
 *
 * PURPOSE:
 *   Implement native environment, compiler, build tool, library, and package
 *   checks without depending on PowerShell execution policy or global PATH edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/output_file.h"
#include "umicom/toolchain/version_inventory.h"

/* Provide the cli has option operation used by this module and its client applications. */
static int umi_cli_has_option(int argc, char **argv, const char *option)
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
 * Provide the cli check option value operation used by this module and its client
 * applications.
 */
static const char *umi_cli_check_option_value(
    int argc,
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

/* Check that cli check options satisfies its contract before another service relies on it. */
/* Retain the earlier parser for review. Version-only diagnostics add a deliberate no-compile mode and reject malformed argument vectors before reading them. */
#if 0
static int umi_cli_check_options_valid(int argc, char **argv)
{
    int index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--gtk") == 0 ||
            strcmp(argv[index], "--github") == 0 ||
            strcmp(argv[index], "--all") == 0 ||
            strcmp(argv[index], "--help") == 0 ||
            strcmp(argv[index], "-h") == 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--project") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1 >= argc || argv[index + 1][0] == '\0' ||
                strncmp(argv[index + 1], "--", 2U) == 0) {
                (void)fprintf(stderr, "--project requires a path.\n");
                return 0;
            }
            ++index;
            continue;
        }
        (void)fprintf(stderr, "Unknown check option: %s\n", argv[index]);
        return 0;
    }
    return 1;
}

#endif
/* The previous version-only parser is retained for review. The active parser
 * additionally validates the evidence destination before any tool command runs. */
#if 0
static int umi_cli_check_options_valid(int argc, char **argv)
{
    int index;
    if (argc < 0 || (argc != 0 && argv == NULL)) return 0;
    /* Validate the complete argument vector before option lookups consume it. */
    for (index = 0; index < argc; ++index)
        if (argv[index] == NULL || argv[index][0] == '\0') return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--versions-only") == 0 ||
            strcmp(argv[index], "--gtk") == 0 ||
            strcmp(argv[index], "--github") == 0 ||
            strcmp(argv[index], "--all") == 0 ||
            strcmp(argv[index], "--help") == 0 ||
            strcmp(argv[index], "-h") == 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--project") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1 >= argc || argv[index + 1][0] == '\0' ||
                strncmp(argv[index + 1], "--", 2U) == 0) {
                (void)fprintf(stderr, "--project requires a path.\n");
                return 0;
            }
            ++index;
            continue;
        }
        (void)fprintf(stderr, "Unknown check option: %s\n", argv[index]);
        return 0;
    }
    return 1;
}

#endif
static int umi_cli_check_options_valid(int argc, char **argv)
{
    int index;
    int report_seen = 0;
    if (argc < 0 || (argc != 0 && argv == NULL)) return 0;
    /* Validate the complete argument vector before option lookups consume it. */
    for (index = 0; index < argc; ++index)
        if (argv[index] == NULL || argv[index][0] == '\0') return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < argc; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--versions-only") == 0 ||
            strcmp(argv[index], "--gtk") == 0 ||
            strcmp(argv[index], "--github") == 0 ||
            strcmp(argv[index], "--all") == 0 ||
            strcmp(argv[index], "--help") == 0 ||
            strcmp(argv[index], "-h") == 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(argv[index], "--report") == 0) {
            if (report_seen || index + 1 >= argc || argv[index + 1][0] == '-' ||
                UmiOutputFileValidatePath(argv[index + 1]) != UMI_STATUS_OK) {
                (void)fputs("--report requires one absolute new-file path.\n", stderr);
                return 0;
            }
            report_seen = 1;
            ++index;
            continue;
        }
        if (strcmp(argv[index], "--project") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1 >= argc || argv[index + 1][0] == '\0' ||
                strncmp(argv[index + 1], "--", 2U) == 0) {
                (void)fprintf(stderr, "--project requires a path.\n");
                return 0;
            }
            ++index;
            continue;
        }
        (void)fprintf(stderr, "Unknown check option: %s\n", argv[index]);
        return 0;
    }
    return 1;
}

/*
 * Provide the cli check require tool operation used by this module and its client
 * applications.
 */
static void umi_cli_check_require_tool(
    UmiToolchainProfile *profile,
    UmiToolKind kind,
    int *missing_count)
{
    UmiToolInfo *tool = umi_toolchain_profile_tool_mutable(profile, kind);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tool == NULL) return;
    tool->required = 1;
    /* Apply this operation only while the related capability or state is available. */
    if (tool->state != UMI_TOOL_VALIDATED) *missing_count += 1;
}

#ifdef _WIN32
/*
 * Provide the cli check msys bash operation used by this module and its client
 * applications.
 */
static int umi_cli_check_msys_bash(
    const UmiToolchainProfile *profile,
    char *out_path,
    size_t capacity)
{
    char root[UMI_PATH_CAPACITY];
    char *separator;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || out_path == NULL || capacity == 0U ||
        profile->root[0] == '\0' ||
        strlen(profile->root) >= sizeof(root)) {
        return 0;
    }
    (void)snprintf(root, sizeof(root), "%s", profile->root);
    separator = strrchr(root, '\\');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL) separator = strrchr(root, '/');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL) return 0;
    *separator = '\0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_fs_join(out_path, capacity, root, "usr/bin/bash.exe") !=
        UMI_STATUS_OK) {
        return 0;
    }
    return umi_fs_is_file(out_path);
}
#endif

/* Provide the cli command check operation used by this module and its client applications. */
/* Tool and compiler probes are different operations. This diagnostic uses the
 * existing Framework discovery request to collect versions without compiling,
 * linking or running a generated program. Its output is inventory, not proof
 * that the project's selected compiler or dependencies can build that project. */
/* Add optional Framework-owned persistence while keeping the earlier console
 * rendering. The old internal signature is retained for source comparison. */
#if 0
static int umi_cli_check_versions(UmiCliContext *context)
#endif
static int umi_cli_check_versions(UmiCliContext *context, const char *report_path)
{
    if (context == NULL) return 2;
    memset(context, 0, sizeof(*context));
    UmiToolchainDiscoveryRequest request;
    umi_toolchain_discovery_request_init(&request);
    request.skip_compile_probe = 1;
    request.diagnostic_sink = umi_cli_diagnostic_sink;
    UmiStatus status = umi_toolchain_discover(&request, &context->discovery);
    /* Save incomplete discovery too. A saved inventory is evidence of what was
     * observed, never a successful qualification or permission to build. */
    if (report_path != NULL) {
        UmiStatus saved = umi_toolchain_version_inventory_save(
            &context->discovery, status, report_path);
        if (saved != UMI_STATUS_OK) {
            (void)fprintf(stderr, "Tool inventory was not saved: %s\n", umi_status_text(saved));
            return 2;
        }
        (void)puts("Tool inventory saved to the requested report file.");
    }
    (void)puts("Umicom tool version inventory\nVersion/target commands ran; no generated program was compiled or launched.");
    (void)printf("Profile: %s\nRoot: %s\n", context->discovery.profile.profile_id,
        context->discovery.profile.root[0] != '\0' ? context->discovery.profile.root : "PATH-based discovery");
    (void)printf("%-20s %-10s %-38s %s\n", "Tool", "Version state", "Version", "Path");
    for (size_t i = 0U; i < context->discovery.profile.tool_count; ++i) {
        const UmiToolInfo *tool = &context->discovery.profile.tools[i];
        if (tool->required || tool->state != UMI_TOOL_MISSING)
            (void)printf("%-20s %-10s %-38s %s\n", umi_tool_kind_name(tool->kind),
                umi_tool_state_text(tool->state), tool->version[0] ? tool->version : "-", tool->path);
    }
    (void)puts("Compile, link, runtime and C23 probes: NOT RUN.\nProject dependencies and source revisions were not qualified.");
    if (status != UMI_STATUS_OK)
        (void)fprintf(stderr, "Tool discovery incomplete: %s\n", umi_status_text(status));
    if (fflush(stdout) != 0 || ferror(stdout)) return 2;
    return status == UMI_STATUS_OK ? 0 : 1;
}

int umi_cli_command_check(UmiCliContext *context, int argc, char **argv)
{
    static const UmiDependencyRequirement base_requirements[] = {
        {"sqlite3", NULL, 1}
    };
    static const UmiDependencyRequirement gtk_requirements[] = {
        {"glib-2.0", NULL, 1},
        {"gobject-2.0", NULL, 1},
        {"gio-2.0", NULL, 1},
        {"gtk4", "4.10", 1},
        {"gtksourceview-5", NULL, 1},
        {"json-glib-1.0", NULL, 1},
        {"libsoup-3.0", NULL, 1},
        {"libcurl", NULL, 1},
        {"sqlite3", NULL, 1}
    };
    UmiDependencyReport dependencies;
    UmiStatus status;
    size_t index;
    static const UmiToolKind analysis_tools[] = {
        UMI_TOOL_CLANG_TIDY,
        UMI_TOOL_CPPCHECK,
        UMI_TOOL_OSV_SCANNER,
        UMI_TOOL_VALGRIND,
        UMI_TOOL_DR_MEMORY
    };
    const char *project_root;
    int check_all;
    int require_gtk;
    int require_github;
    int required_path_missing = 0;
    int supplemental_tools_missing = 0;

    /* Apply this operation only while the related capability or state is available. */
    if (!umi_cli_check_options_valid(argc, argv)) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_cli_has_option(argc, argv, "--help") ||
        umi_cli_has_option(argc, argv, "-h")) {
        (void)puts(
            "Usage: umicom check [--all] [--gtk] [--github] "
            "[--project PATH]\n"
            "       umicom doctor --versions-only [--report ABSOLUTE_NEW_FILE]\n"
            "  --versions-only  Capture discovered tool versions without compile/link/run probes.\n"
            "                   Invokes tool version/target commands. This is not qualification.\n"
            "  --report   Save version-only JSON evidence; existing files are never replaced.\n"
            "  --all      Check desktop libraries and GitHub CLI as well as "
            "the core toolchain.\n"
            "  --project  Also require PATH/CMakePresets.json.");
        return 0;
    }
    /* Preserve the original no-report admission for review. Evidence output
     * is the sole additional option accepted by version-only discovery. */
#if 0
    if (umi_cli_has_option(argc, argv, "--versions-only")) {
        if (argc != 1) {
            (void)fputs("--versions-only must be used alone. Use a separate full doctor check for qualification.\n", stderr);
            return 2;
        }
        return umi_cli_check_versions(context);
    }
#endif
    const char *report_path = umi_cli_check_option_value(argc, argv, "--report");
    if (umi_cli_has_option(argc, argv, "--versions-only")) {
        if (argc != (report_path != NULL ? 3 : 1)) {
            (void)fputs("--versions-only accepts only an optional --report path.\n", stderr);
            return 2;
        }
        return umi_cli_check_versions(context, report_path);
    }
    if (report_path != NULL) {
        (void)fputs("--report requires --versions-only. Full doctor checks use separate qualification evidence.\n", stderr);
        return 2;
    }
    check_all = umi_cli_has_option(argc, argv, "--all");
    require_gtk = check_all || umi_cli_has_option(argc, argv, "--gtk");
    require_github = check_all || umi_cli_has_option(argc, argv, "--github");
    project_root = umi_cli_check_option_value(argc, argv, "--project");

    status = umi_cli_context_prepare(context,
                                     project_root,
                                     require_gtk,
                                     require_github);
    /* Package checks below always use pkg-config, including the small
     * headless SQLite check, so report it as a required tool here. */
    umi_cli_check_require_tool(
        &context->discovery.profile,
        UMI_TOOL_PKG_CONFIG,
        &supplemental_tools_missing);
    /* Apply this branch only when its contract condition is satisfied. */
    if (check_all) {
        umi_cli_check_require_tool(
            &context->discovery.profile,
            UMI_TOOL_CPACK,
            &supplemental_tools_missing);
        umi_cli_check_require_tool(
            &context->discovery.profile,
            UMI_TOOL_GDB,
            &supplemental_tools_missing);
        /* Apply this branch only when its contract condition is satisfied. */
        if (context->discovery.profile.selected_cpp_compiler < UMI_TOOL_COUNT) {
            umi_cli_check_require_tool(
                &context->discovery.profile,
                context->discovery.profile.selected_cpp_compiler,
                &supplemental_tools_missing);
        }
    }
    (void)printf("Umicom Environment Check\n\n");
    (void)printf("Profile: %s\n",
                 context->discovery.profile.profile_id);
    (void)printf("Family: %s\n",
                 umi_toolchain_family_text(
                     context->discovery.profile.family));
    (void)printf("Root: %s\n\n",
                 context->discovery.profile.root[0] != '\0'
                     ? context->discovery.profile.root
                     : "PATH-based environment");

    (void)puts("Tools:");
    (void)printf("%-20s %-10s %-38s %s\n",
                 "Tool", "State", "Version", "Path");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < context->discovery.profile.tool_count; ++index) {
        const UmiToolInfo *tool = &context->discovery.profile.tools[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (tool->required || tool->state != UMI_TOOL_MISSING) {
            (void)printf("%-20s %-10s %-38s %s\n",
                         umi_tool_kind_name(tool->kind),
                         umi_tool_state_text(tool->state),
                         tool->version[0] != '\0' ? tool->version : "-",
                         tool->path);
        }
    }

    (void)puts("\nOptional analysis tools:");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < sizeof(analysis_tools) / sizeof(analysis_tools[0]);
         ++index) {
        const UmiToolInfo *tool = umi_toolchain_profile_tool(
            &context->discovery.profile, analysis_tools[index]);
        (void)printf(
            "%-20s %-10s %-38s %s\n",
            umi_tool_kind_name(analysis_tools[index]),
            tool != NULL ? umi_tool_state_text(tool->state) : "MISSING",
            tool != NULL && tool->version[0] != '\0' ? tool->version : "-",
            tool != NULL && tool->path[0] != '\0' ? tool->path :
                "optional; install to enable the related external gate");
    }

    (void)puts("\nRequired paths:");
#ifdef _WIN32
    {
        char bash_path[UMI_PATH_CAPACITY];
        int bash_found = umi_cli_check_msys_bash(
            &context->discovery.profile, bash_path, sizeof(bash_path));
        (void)printf("%-10s %s\n",
                     bash_found ? "PASS" : "MISSING",
                     bash_found ? bash_path : "MSYS2 usr/bin/bash.exe");
        /* Apply this branch only when its contract condition is satisfied. */
        if (!bash_found) required_path_missing = 1;
    }
#endif
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_root != NULL) {
        char preset_path[UMI_PATH_CAPACITY];
        int preset_found =
            umi_fs_join(preset_path, sizeof(preset_path),
                        project_root, "CMakePresets.json") == UMI_STATUS_OK &&
            umi_fs_is_file(preset_path);
        (void)printf("%-10s %s\n",
                     preset_found ? "PASS" : "MISSING",
                     preset_found ? preset_path :
                         "Project CMakePresets.json");
        /* Apply this branch only when its contract condition is satisfied. */
        if (!preset_found) required_path_missing = 1;
    }
    (void)printf("\nCompile probe: %s\n",
                 context->discovery.compile_probe_passed ? "PASS" : "FAIL");
    (void)printf("Link probe:    %s\n",
                 context->discovery.link_probe_passed ? "PASS" : "FAIL");
    (void)printf("Runtime probe: %s\n",
                 context->discovery.runtime_probe_passed ? "PASS" : "FAIL");

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || required_path_missing ||
        supplemental_tools_missing > 0) {
        (void)fprintf(stderr,
                      "\nEnvironment is not ready: %s\n",
                      status != UMI_STATUS_OK
                          ? umi_status_text(status)
                          : required_path_missing
                              ? "required path missing"
                              : "required developer tool missing");
        return 1;
    }

    status = umi_dependency_check(
        &context->discovery.profile,
        &context->environment,
        require_gtk ? gtk_requirements : base_requirements,
        require_gtk
            ? sizeof(gtk_requirements) / sizeof(gtk_requirements[0])
            : sizeof(base_requirements) / sizeof(base_requirements[0]),
        &dependencies
    );
    (void)puts("\nPackages:");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < dependencies.count; ++index) {
        const UmiDependencyStatus *item = &dependencies.items[index];
        const UmiDependencyRequirement *requirement = require_gtk
            ? &gtk_requirements[index]
            : &base_requirements[index];
        const char *package_state = item->available
            ? "PASS"
            : item->version[0] != '\0' ? "VERSION" : "MISSING";
        (void)printf("%-24s %-8s %-16s required %s\n",
                     item->package_name,
                     package_state,
                     item->version[0] != '\0' ? item->version : "-",
                     requirement->minimum_version != NULL
                         ? requirement->minimum_version
                         : "any validated version");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "\nMissing required packages: %zu\n",
                      dependencies.required_missing);
        return 1;
    }
    (void)puts("\nEnvironment is ready.");
    return 0;
}

/*
 * Provide the cli command environment operation used by this module and its client
 * applications.
 */
int umi_cli_command_environment(UmiCliContext *context, int argc, char **argv)
{
    size_t index;
    (void)argc;
    (void)argv;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_cli_context_prepare(context, NULL, 0, 0) != UMI_STATUS_OK) {
        return 1;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < context->environment.count; ++index) {
        (void)printf("%s=%s\n",
                     context->environment.entries[index].name,
                     context->environment.entries[index].value);
    }
    return 0;
}
