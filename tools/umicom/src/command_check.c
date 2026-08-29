/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_check.c
 *
 * PURPOSE:
 *   Implement native environment, compiler, build tool, library, and package
 *   checks without depending on PowerShell execution policy or global PATH edits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

static int umi_cli_has_option(int argc, char **argv, const char *option)
{
    int index;
    for (index = 0; index < argc; ++index) {
        if (strcmp(argv[index], option) == 0) return 1;
    }
    return 0;
}

static const char *umi_cli_check_option_value(
    int argc,
    char **argv,
    const char *option)
{
    int index;
    for (index = 0; index + 1 < argc; ++index) {
        if (strcmp(argv[index], option) == 0) return argv[index + 1];
    }
    return NULL;
}

static int umi_cli_check_options_valid(int argc, char **argv)
{
    int index;
    for (index = 0; index < argc; ++index) {
        if (strcmp(argv[index], "--gtk") == 0 ||
            strcmp(argv[index], "--github") == 0 ||
            strcmp(argv[index], "--all") == 0 ||
            strcmp(argv[index], "--help") == 0 ||
            strcmp(argv[index], "-h") == 0) {
            continue;
        }
        if (strcmp(argv[index], "--project") == 0) {
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

static void umi_cli_check_require_tool(
    UmiToolchainProfile *profile,
    UmiToolKind kind,
    int *missing_count)
{
    UmiToolInfo *tool = umi_toolchain_profile_tool_mutable(profile, kind);
    if (tool == NULL) return;
    tool->required = 1;
    if (tool->state != UMI_TOOL_VALIDATED) *missing_count += 1;
}

#ifdef _WIN32
static int umi_cli_check_msys_bash(
    const UmiToolchainProfile *profile,
    char *out_path,
    size_t capacity)
{
    char root[UMI_PATH_CAPACITY];
    char *separator;

    if (profile == NULL || out_path == NULL || capacity == 0U ||
        profile->root[0] == '\0' ||
        strlen(profile->root) >= sizeof(root)) {
        return 0;
    }
    (void)snprintf(root, sizeof(root), "%s", profile->root);
    separator = strrchr(root, '\\');
    if (separator == NULL) separator = strrchr(root, '/');
    if (separator == NULL) return 0;
    *separator = '\0';
    if (umi_fs_join(out_path, capacity, root, "usr/bin/bash.exe") !=
        UMI_STATUS_OK) {
        return 0;
    }
    return umi_fs_is_file(out_path);
}
#endif

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
    const char *project_root;
    int check_all;
    int require_gtk;
    int require_github;
    int required_path_missing = 0;
    int supplemental_tools_missing = 0;

    if (!umi_cli_check_options_valid(argc, argv)) return 2;
    if (umi_cli_has_option(argc, argv, "--help") ||
        umi_cli_has_option(argc, argv, "-h")) {
        (void)puts(
            "Usage: umicom check [--all] [--gtk] [--github] "
            "[--project PATH]\n"
            "  --all      Check desktop libraries and GitHub CLI as well as "
            "the core toolchain.\n"
            "  --project  Also require PATH/CMakePresets.json.");
        return 0;
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
    if (check_all) {
        umi_cli_check_require_tool(
            &context->discovery.profile,
            UMI_TOOL_CPACK,
            &supplemental_tools_missing);
        umi_cli_check_require_tool(
            &context->discovery.profile,
            UMI_TOOL_GDB,
            &supplemental_tools_missing);
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
    for (index = 0U; index < context->discovery.profile.tool_count; ++index) {
        const UmiToolInfo *tool = &context->discovery.profile.tools[index];
        if (tool->required || tool->state != UMI_TOOL_MISSING) {
            (void)printf("%-20s %-10s %-38s %s\n",
                         umi_tool_kind_name(tool->kind),
                         umi_tool_state_text(tool->state),
                         tool->version[0] != '\0' ? tool->version : "-",
                         tool->path);
        }
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
        if (!bash_found) required_path_missing = 1;
    }
#endif
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
        if (!preset_found) required_path_missing = 1;
    }
    (void)printf("\nCompile probe: %s\n",
                 context->discovery.compile_probe_passed ? "PASS" : "FAIL");
    (void)printf("Link probe:    %s\n",
                 context->discovery.link_probe_passed ? "PASS" : "FAIL");
    (void)printf("Runtime probe: %s\n",
                 context->discovery.runtime_probe_passed ? "PASS" : "FAIL");

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
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "\nMissing required packages: %zu\n",
                      dependencies.required_missing);
        return 1;
    }
    (void)puts("\nEnvironment is ready.");
    return 0;
}

int umi_cli_command_environment(UmiCliContext *context, int argc, char **argv)
{
    size_t index;
    (void)argc;
    (void)argv;
    if (umi_cli_context_prepare(context, NULL, 0, 0) != UMI_STATUS_OK) {
        return 1;
    }
    for (index = 0U; index < context->environment.count; ++index) {
        (void)printf("%s=%s\n",
                     context->environment.entries[index].name,
                     context->environment.entries[index].value);
    }
    return 0;
}
