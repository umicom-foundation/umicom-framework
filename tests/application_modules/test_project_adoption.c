/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_project_adoption.c
 * PURPOSE: Check application previews become untrusted IDE projects without filesystem writes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/application_scaffold.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

/* Each case receives its own fixture from CMake. Large public records are heap
 * allocated to keep this regression useful on hosts with small default stacks. */
int main(int argc, char **argv)
{
    int failed = 0;
    UmiApplicationScaffoldPlan *plan = NULL;
    UmiDeveloperProjectModel *model = calloc(1U, sizeof *model);
    UmiBuildProfile *profile = calloc(1U, sizeof *profile);
    char parent[UMI_PATH_CAPACITY], templates[UMI_PATH_CAPACITY], target[UMI_PATH_CAPACITY];
    char identifier[202];
    CHECK(argc == 3 && model != NULL && profile != NULL);
    CHECK(umi_path_join(argv[2], "output", parent, sizeof parent) == UMI_STATUS_OK);
    CHECK(umi_path_join(argv[2], "templates", templates, sizeof templates) == UMI_STATUS_OK);
    CHECK(umi_path_join(parent, "weather", target, sizeof target) == UMI_STATUS_OK);
    UmiApplicationScaffoldRequest request = {
        templates, parent, "weather", "Weather", "org.example.weather", strcmp(argv[1], "console") != 0
    };
    if (strcmp(argv[1], "identifier_capacity") == 0) {
        memset(identifier, 'a', 198U); identifier[3] = '.'; identifier[198] = '\0';
        request.application_id = identifier;
    }
    CHECK(umi_application_scaffold_plan(&request, &plan) == UMI_STATUS_OK);
    if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiDeveloperApplicationScaffoldProject(NULL, model, profile) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeveloperApplicationScaffoldProject(plan, NULL, profile) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeveloperApplicationScaffoldProject(plan, model, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(model->structure_size == 0U && profile->parallel_jobs == 0U);
    } else if (strcmp(argv[1], "identifier_capacity") == 0) {
        strcpy(model->display_name, "unchanged");
        strcpy(profile->profile_id, "unchanged");
        CHECK(UmiDeveloperApplicationScaffoldProject(plan, model, profile) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(model->display_name, "unchanged") == 0);
        CHECK(strcmp(profile->profile_id, "unchanged") == 0);
    } else {
        CHECK(UmiDeveloperApplicationScaffoldProject(plan, model, profile) == UMI_STATUS_OK);
        CHECK(strcmp(model->project_id, "org.example.weather") == 0);
        CHECK(strcmp(model->entry_point, request.gtk_frontend ? "src/gtk.c" : "src/console.c") == 0);
        CHECK(umi_path_equal(model->root, target));
        CHECK(umi_path_equal(profile->source_directory, target));
        CHECK(model->trusted == 0 && model->generated == 0);
        CHECK(profile->run_program[0] == '\0' && profile->run_argument[0] == '\0');
        CHECK(profile->build_testing && profile->parallel_jobs == 2U);
        CHECK(strcmp(profile->build_directory, "build/umicom-development") == 0);
        CHECK(umi_developer_project_model_validate(model, NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_build_profile_validate(profile, NULL, 0U) == UMI_STATUS_OK);
    }
    CHECK(!umi_fs_exists(target));
cleanup:
    umi_application_scaffold_destroy(plan);
    free(model); free(profile);
    return failed;
}
