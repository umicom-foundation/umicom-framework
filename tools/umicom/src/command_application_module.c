/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_application_module.c
 * PURPOSE: Expose reviewed native application-module creation without implicit Git changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/scaffold/application_module.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>

/* Framework now resolves the resources for CLI and graphical consumers alike.
 * Retain the original CLI-only resolver for review of installed path behaviour. */
#if 0
#ifndef UMICOM_APPLICATION_MODULE_TEMPLATE_ROOT
#define UMICOM_APPLICATION_MODULE_TEMPLATE_ROOT ""
#endif
#ifndef UMICOM_APPLICATION_MODULE_TEMPLATE_INSTALL_DIR
#define UMICOM_APPLICATION_MODULE_TEMPLATE_INSTALL_DIR "share/umicom/templates/application_module"
#endif

/* Resolve installed resources from the running executable. A source build has
 * an explicit compile-time fallback; neither path depends on a hidden user
 * directory or on where the command happened to be launched. */
static UmiStatus module_template_root(char *output, size_t capacity)
{
    char image[UMI_PATH_CAPACITY], bin[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY];
    UmiStatus status = umi_fs_executable_path(image, sizeof(image));
    if (status == UMI_STATUS_OK) status = umi_path_parent(image, bin, sizeof(bin));
    if (status == UMI_STATUS_OK) status = umi_path_parent(bin, prefix, sizeof(prefix));
    if (status == UMI_STATUS_OK) {
        const char *installed = UMICOM_APPLICATION_MODULE_TEMPLATE_INSTALL_DIR;
        status = umi_path_is_absolute(installed)
            ? umi_path_copy(output, capacity, installed)
            : umi_path_join(prefix, installed, output, capacity);
        if (status == UMI_STATUS_OK && umi_fs_is_directory(output)) return status;
    }
    status = umi_path_copy(output, capacity, UMICOM_APPLICATION_MODULE_TEMPLATE_ROOT);
    if (status == UMI_STATUS_OK && umi_path_is_absolute(output) &&
        umi_fs_is_directory(output)) return status;
    output[0] = '\0';
    return UMI_STATUS_NOT_FOUND;
}

#endif
static UmiStatus module_template_root(char *output, size_t capacity)
{
    return umi_application_scaffold_template_root(output, capacity);
}

/* Parse every option exactly once. Legacy create options are deliberately not
 * inherited here: --push or --remote must not turn source creation into an
 * unexpected network operation. The existing creator retains its own flags. */
int umi_cli_command_application_module(int argc, char **argv)
{
    UmiApplicationScaffoldRequest request = {0};
    UmiApplicationScaffoldPlan *plan = NULL;
    UmiApplicationScaffoldSummary summary;
    UmiApplicationScaffoldResult result;
    char current[UMI_PATH_CAPACITY], parent[UMI_PATH_CAPACITY], templates[UMI_PATH_CAPACITY];
    const char *parent_argument = NULL, *template_argument = NULL, *show = NULL;
    bool module_flag = false, apply = false, dry_run = false, console_only = false;
    UmiStatus status;
    int exit_code = 0;
    if (argc < 2 || argv[0][0] == '-') goto usage;
    request.application_name = argv[0];
    request.gtk_frontend = true;
    for (int i = 1; i < argc; ++i) {
        const char *argument = argv[i];
        if (strcmp(argument, "--module") == 0) {
            if (module_flag) goto usage;
            module_flag = true;
        } else if (strcmp(argument, "--apply") == 0) {
            if (apply) goto usage;
            apply = true;
        } else if (strcmp(argument, "--dry-run") == 0) {
            if (dry_run) goto usage;
            dry_run = true;
        } else if (strcmp(argument, "--console-only") == 0) {
            if (console_only) goto usage;
            console_only = true;
            request.gtk_frontend = false;
        } else {
            const char **destination = NULL;
            if (strcmp(argument, "--parent") == 0) destination = &parent_argument;
            else if (strcmp(argument, "--slug") == 0) destination = &request.slug;
            else if (strcmp(argument, "--id") == 0) destination = &request.application_id;
            else if (strcmp(argument, "--template-root") == 0) destination = &template_argument;
            else if (strcmp(argument, "--show") == 0) destination = &show;
            if (destination == NULL || *destination != NULL || i + 1 >= argc ||
                argv[i + 1][0] == '\0' || argv[i + 1][0] == '-') goto usage;
            *destination = argv[++i];
        }
    }
    if (!module_flag || parent_argument == NULL || request.slug == NULL ||
        request.application_id == NULL || (apply && (dry_run || show != NULL))) goto usage;

    /* Resolve relative CLI paths once, then hand absolute owned paths to the
     * service. The service itself never guesses a destination folder. */
    status = umi_fs_current_directory(current, sizeof(current));
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(parent_argument, current, parent, sizeof(parent));
    if (status == UMI_STATUS_OK)
        status = template_argument != NULL
            ? umi_path_absolute(template_argument, current, templates, sizeof(templates))
            : module_template_root(templates, sizeof(templates));
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot resolve source folders: %s. Use --template-root if needed.\n",
            umi_status_text(status));
        return 1;
    }
    request.destination_parent = parent;
    request.template_root = templates;
    status = umi_application_scaffold_plan(&request, &plan);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot preview application: %s\n", umi_status_text(status));
        return 1;
    }
    status = umi_application_scaffold_summary(plan, &summary);
    if (status != UMI_STATUS_OK) { umi_application_scaffold_destroy(plan); return 1; }
    (void)printf("Application: %s\nDestination: %s\nBuild option: %s\nFiles: %zu\n",
        summary.module.name, summary.root, summary.module.option, summary.file_count);
    bool found = show == NULL;
    for (size_t i = 0U; i < summary.file_count; ++i) {
        UmiApplicationScaffoldFile file;
        status = umi_application_scaffold_file(plan, i, &file);
        if (status != UMI_STATUS_OK) { exit_code = 1; break; }
        if (show == NULL) (void)printf("  %s (%zu bytes)\n", file.relative_path, file.length);
        else if (strcmp(show, file.relative_path) == 0) {
            found = true;
            (void)printf("\n%s\n", file.text);
        }
    }
    if (!found) {
        (void)fprintf(stderr, "The requested preview path is not in this application.\n");
        exit_code = 2;
    }
    if (exit_code == 0 && apply) {
        status = umi_application_scaffold_apply(plan, &result);
        (void)printf("Created directories: %zu\nCreated files: %zu\nCompleted files: %zu\n",
            result.created_directories, result.created_files, result.completed_files);
        if (status != UMI_STATUS_OK) {
            (void)fprintf(stderr, "Creation stopped at %s: %s\n"
                "Partial work is retained. Inspect it before choosing a new destination.\n",
                result.last_path, umi_status_text(status));
            exit_code = 1;
        } else (void)puts("Application source created. No Git changes, build or launch performed.");
    } else if (exit_code == 0)
        (void)puts("Preview only. Review --show PATH, then use --apply to create the source.");
    umi_application_scaffold_destroy(plan);
    return exit_code;
usage:
    (void)fputs("Usage: umicom repo create NAME --module --parent DIRECTORY --slug SLUG --id ID\n"
        "  [--console-only] [--template-root DIRECTORY] [--show PATH | --dry-run | --apply]\n",
        stderr);
    return 2;
}
