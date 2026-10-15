/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_application_catalogue.c
 * PURPOSE: Expose registered application inventory without creating or updating repositories.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/scaffold/application_catalogue.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>

/* Resolve the selected root once. The service uses Git only to read its
 * registration file and does not need a compiler, SDK, account or remote access. */
int umi_cli_command_application_catalogue(int argc, char **argv)
{
    if (argc > 1 || (argc == 1 && (argv[0][0] == '\0' || argv[0][0] == '-'))) {
        (void)fputs("Usage: umicom repo modules [SUITE_DIRECTORY]\n", stderr);
        return 2;
    }
    char current[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY];
    UmiStatus status = umi_fs_current_directory(current, sizeof current);
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(argc == 1 ? argv[0] : current, current, root, sizeof root);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot resolve suite directory: %s\n", umi_status_text(status));
        return 1;
    }
    UmiApplicationCatalogue *catalogue = NULL;
    UmiApplicationCatalogueIssue issue;
    UmiApplicationCatalogueSummary summary;
    status = UmiApplicationCatalogueLoad(root, NULL, &catalogue, &issue);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect application registrations: %s\n", umi_status_text(status));
        if (issue.path[0] != '\0') (void)fprintf(stderr, "Path: %s\n", issue.path);
        if (issue.conflicting_path[0] != '\0')
            (void)fprintf(stderr, "Conflicts with: %s\n", issue.conflicting_path);
        return 1;
    }
    status = UmiApplicationCatalogueSummaryRead(catalogue, &summary);
    if (status != UMI_STATUS_OK) { UmiApplicationCatalogueDestroy(catalogue); return 1; }
    (void)printf("Suite: %s\nRegistered applications: %zu\nReady manifests: %zu\nUnavailable manifests: %zu\n",
        summary.root, summary.count, summary.ready, summary.unavailable);
    for (size_t i = 0U; i < summary.count; ++i) {
        const UmiApplicationCatalogueEntry *entry = UmiApplicationCatalogueAt(catalogue, i);
        if (entry == NULL) continue;
        (void)printf("\n%s: %s\n", entry->path, umi_status_text(entry->manifest_status));
        if (entry->manifest_status == UMI_STATUS_OK) {
            (void)printf("  %s (%s)\n  Build option: %s\n  Install component: %s\n",
                entry->module.name, entry->module.id, entry->module.option, entry->module.component);
            if (entry->module.native_executable[0] != '\0')
                (void)printf("  Desktop: %s\n", entry->module.native_executable);
            if (entry->module.console_executable[0] != '\0')
                (void)printf("  Console: %s\n", entry->module.console_executable);
        }
    }
    (void)puts("\nLocal registration and manifest evidence only. No repository changes or network requests made.");
    int result = summary.unavailable == 0U ? 0 : 1;
    UmiApplicationCatalogueDestroy(catalogue);
    return result;
}
