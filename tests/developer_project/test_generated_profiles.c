/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generated_profiles.c
 *
 * PURPOSE:
 *   Verify generated simulation profiles survive translation into build commands without native launch defaults.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/service.h"
#include "umicom/build/project_profile.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "line %d: %s\n", __LINE__, #value); result = EXIT_FAILURE; goto done; } } while (0)
int main(void)
{
    UmiDeveloperProjectService *service = NULL;
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
    char cwd[UMI_PATH_CAPACITY];
    int result = EXIT_SUCCESS;
    const struct { const char *id; const char *preset; const char *directory; } cases[] = {
        {"developer.template.vhdl-cpu", "simulation", "build/simulation"},
        {"developer.template.umicom-c-preview", "preview", "build/preview"}
    };
    CHECK(umi_developer_project_service_create(&service) == UMI_STATUS_OK);
    CHECK(umi_fs_current_directory(cwd, sizeof(cwd)) == UMI_STATUS_OK);
    umi_developer_project_generation_request_init(&request);
    strcpy(request.application_name, "Learning Project");
    strcpy(request.application_id, "org.umicom.learning");
    strcpy(request.repository_name, "learning-project");
    strcpy(request.target_name, "learning_project");
    CHECK(umi_fs_join(request.project_root, sizeof(request.project_root), cwd,
        "profile preview only") == UMI_STATUS_OK);
    request.dry_run = 1;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        strcpy(request.template_id, cases[index].id);
        CHECK(umi_developer_project_service_generate(service, &request, &report, &model) == UMI_STATUS_OK);
        CHECK(model.kind == UMI_DEVELOPER_PROJECT_META);
        CHECK(strcmp(model.preset, cases[index].preset) == 0);
        CHECK(strcmp(model.build_directory, cases[index].directory) == 0);
        CHECK(model.executable[0] == '\0' && model.install_prefix[0] == '\0');
        CHECK(UmiBuildProfileFromProject(&model, &profile) == UMI_STATUS_OK);
        CHECK(strcmp(profile.preset, cases[index].preset) == 0);
        CHECK(strcmp(profile.build_directory, cases[index].directory) == 0);
        CHECK(profile.run_program[0] == '\0');
    }
    /* A later native project must still receive its existing platform default. */
    strcpy(request.template_id, "developer.template.c23-console");
    CHECK(umi_developer_project_service_generate(service, &request, &report, &model) == UMI_STATUS_OK);
    CHECK(model.kind == UMI_DEVELOPER_PROJECT_EXECUTABLE);
#ifdef _WIN32
    CHECK(strcmp(model.preset, "windows-ucrt64-debug") == 0);
    CHECK(strcmp(model.executable, "build/windows-ucrt64-debug/bin/learning_project.exe") == 0);
#else
    CHECK(strcmp(model.preset, "linux-debug") == 0);
    CHECK(strcmp(model.executable, "build/linux-debug/bin/learning_project") == 0);
#endif
    CHECK(UmiBuildProfileFromProject(&model, &profile) == UMI_STATUS_OK);
    CHECK(strcmp(profile.run_program, model.executable) == 0);
done:
    umi_developer_project_service_destroy(service);
    return result;
}
