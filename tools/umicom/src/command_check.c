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

static int umi_cli_has_option(int argc, char **argv, const char *option)
{
    int index;
    for (index = 0; index < argc; ++index) {
        if (strcmp(argv[index], option) == 0) return 1;
    }
    return 0;
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
        {"gtk4", NULL, 1},
        {"gtksourceview-5", NULL, 1},
        {"json-glib-1.0", NULL, 1},
        {"libsoup-3.0", NULL, 1},
        {"libcurl", NULL, 1},
        {"sqlite3", NULL, 1}
    };
    UmiDependencyReport dependencies;
    UmiStatus status;
    size_t index;
    int require_gtk = umi_cli_has_option(argc, argv, "--gtk");
    int require_github = umi_cli_has_option(argc, argv, "--github");

    status = umi_cli_context_prepare(context,
                                     NULL,
                                     require_gtk,
                                     require_github);
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

    for (index = 0U; index < context->discovery.profile.tool_count; ++index) {
        const UmiToolInfo *tool = &context->discovery.profile.tools[index];
        if (tool->required || tool->state != UMI_TOOL_MISSING) {
            (void)printf("%-24s %-8s %s\n",
                         umi_tool_kind_name(tool->kind),
                         umi_tool_state_text(tool->state),
                         tool->path);
        }
    }
    (void)printf("\nCompile probe: %s\n",
                 context->discovery.compile_probe_passed ? "PASS" : "FAIL");
    (void)printf("Link probe:    %s\n",
                 context->discovery.link_probe_passed ? "PASS" : "FAIL");
    (void)printf("Runtime probe: %s\n",
                 context->discovery.runtime_probe_passed ? "PASS" : "FAIL");

    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "\nEnvironment is not ready: %s\n",
                      umi_status_text(status));
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
        (void)printf("%-24s %-8s %s\n",
                     item->package_name,
                     item->available ? "PASS" : "MISSING",
                     item->version);
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
