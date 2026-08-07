#include "umicom/scaffold/scaffold.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define REMOVE_TREE(command_path) do { char command[2048]; (void)snprintf(command, sizeof(command), "rmdir /S /Q \"%s\"", command_path); (void)system(command); } while (0)
#else
#define REMOVE_TREE(command_path) do { char command[2048]; (void)snprintf(command, sizeof(command), "rm -rf \"%s\"", command_path); (void)system(command); } while (0)
#endif

#ifndef UMICOM_TEST_TEMPLATE_ROOT
#define UMICOM_TEST_TEMPLATE_ROOT "templates"
#endif

int main(void)
{
    const char *workspace = "umicom-scaffold-test-workspace";
    UmiScaffoldRequest request;
    UmiScaffoldReport report;
    REMOVE_TREE(workspace);
    assert(umi_scaffold_copy_workspace_template(UMICOM_TEST_TEMPLATE_ROOT, workspace) == UMI_STATUS_OK);
    assert(umi_scaffold_is_workspace(workspace));
    request.workspace_root = workspace;
    request.application_name = "Test Application";
    request.frontends = UMI_SCAFFOLD_FRONTEND_CONSOLE | UMI_SCAFFOLD_FRONTEND_WEB;
    assert(umi_scaffold_create_application(&request, &report) == UMI_STATUS_OK);
    assert(strcmp(report.application_slug, "test-application") == 0);
    assert(report.files_created >= 7U);
    REMOVE_TREE(workspace);
    return 0;
}
