/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/project_creator.c
 * PURPOSE: Create a fresh CMake project through the same Framework service as Studio.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/new_project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    UmiDeveloperProjectService *service = NULL;
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        puts("Usage: umicom-learning-project-creator <absolute-new-directory>");
        puts("Creates a C23 project named Umicom Notes. Existing directories are refused.");
        return argc == 2 ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    if (strlen(argv[1]) >= sizeof(request.project_root)) return EXIT_FAILURE;
    umi_developer_project_generation_request_init(&request);
    strcpy(request.application_name, "Umicom Notes");
    strcpy(request.application_id, "org.umicom.notes");
    strcpy(request.repository_name, "umicom-notes");
    strcpy(request.target_name, "umicom_notes");
    strcpy(request.template_id, "developer.template.c23-console");
    strcpy(request.project_root, argv[1]);
    UmiStatus status = umi_developer_project_service_create(&service);
    if (status == UMI_STATUS_OK)
        status = UmiDeveloperProjectCreateNew(service, &request, &report, &model, &profile);
    if (status == UMI_STATUS_OK)
        printf("Created %zu files in %s\nConfigure preset: %s\nRun program: %s\n",
            report.files_created, model.root, profile.preset, profile.run_program);
    else fprintf(stderr, "Project was not created: %s\n", umi_status_text(status));
    umi_developer_project_service_destroy(service);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
