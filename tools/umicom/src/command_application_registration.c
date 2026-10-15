/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_application_registration.c
 * PURPOSE: Preview application registration conflicts without changing Git or files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/scaffold/application_catalogue.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>

/* Resolve both selected paths once. The catalogue and parser own all copied
 * metadata, so no displayed value depends on a later change to a text field. */
int umi_cli_command_application_registration(int argc, char **argv)
{
    if (argc != 3 || argv[0][0] == '\0' || argv[1][0] == '\0' || argv[2][0] == '\0') {
        (void)fputs("Usage: umicom repo check-module SUITE_DIRECTORY MANIFEST applications/slug\n", stderr);
        return 2;
    }
    char current[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], manifest[UMI_PATH_CAPACITY];
    UmiStatus status = umi_fs_current_directory(current, sizeof current);
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(argv[0], current, root, sizeof root);
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(argv[1], current, manifest, sizeof manifest);
    UmiApplicationModuleDescriptor module;
    if (status == UMI_STATUS_OK) status = umi_application_module_load(manifest, &module);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect proposed module: %s\n", umi_status_text(status));
        return 1;
    }
    UmiApplicationCatalogue *catalogue = NULL;
    UmiApplicationCatalogueIssue issue;
    status = UmiApplicationCatalogueLoad(root, NULL, &catalogue, &issue);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot review existing registrations: %s\n", umi_status_text(status));
        if (issue.path[0] != '\0') (void)fprintf(stderr, "Path: %s\n", issue.path);
        if (issue.conflicting_path[0] != '\0')
            (void)fprintf(stderr, "Conflicts with: %s\n", issue.conflicting_path);
        return 1;
    }
    UmiApplicationCandidateReport report;
    status = UmiApplicationCatalogueCheckCandidate(catalogue, argv[2], &module, &report);
    (void)printf("Application: %s\nIdentity: %s\nProposed path: %s\nBuild option: %s\n"
        "Installer component: %s\nChecked manifests: %zu\n%s\n",
        module.name, module.id, argv[2], module.option, module.component, report.checked,
        UmiApplicationCandidateReasonText(report.reason));
    if (report.conflicting_path[0] != '\0')
        (void)printf("Existing registration: %s\n", report.conflicting_path);
    /* A successful preview is not an automatic submodule-add request. A future
     * reviewed registration workflow must recheck after any repository edits. */
    (void)puts("Source snapshot only. No submodule, index, commit or remote was changed.");
    UmiApplicationCatalogueDestroy(catalogue);
    return status == UMI_STATUS_OK ? 0 : 1;
}
