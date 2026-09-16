/*-----------------------------------------------------------------------------
 * Umicom Framework / Studio qualification
 * File: framework/tests/developer_project/test_project_input_boundaries.c
 * PURPOSE: Reject malformed and injectable generation input without modifying output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)

#include "umicom/developer_project/new_project.h"
#include "umicom/build/project_profile.h"
#include "umicom/platform/filesystem.h"
int main(void)
{
    UmiDeveloperProjectGenerationRequest request, changed;
    UmiDeveloperProjectGenerationPlan *plan = calloc(1U, sizeof(*plan));
    UmiBuildProfile profile, unchanged;
    CHECK(plan != NULL);
    umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id, "developer.template.c23-console");
    strcpy(request.application_name, "Umicom Notes");
    strcpy(request.application_id, "org.umicom.notes");
    strcpy(request.repository_name, "umicom-notes");
    strcpy(request.target_name, "umicom_notes");
    strcpy(request.project_root, "/tmp/Umicom Notes");
    CHECK(umi_developer_project_generation_request_validate(&request, NULL, 0U) == UMI_STATUS_OK);
    changed = request; memset(changed.application_name, 'X', sizeof(changed.application_name));
    CHECK(umi_developer_project_generation_request_validate(&changed, NULL, 0U) != UMI_STATUS_OK);
    changed = request; strcpy(changed.target_name, "notes)\nexecute_process(COMMAND bad");
    CHECK(umi_developer_project_generation_request_validate(&changed, NULL, 0U) != UMI_STATUS_OK);
    changed = request; strcpy(changed.application_name, "Notes\"; wrong");
    CHECK(umi_developer_project_generation_request_validate(&changed, NULL, 0U) != UMI_STATUS_OK);
    changed = request; memset(changed.project_root, 'X', sizeof(changed.project_root));
    CHECK(umi_developer_project_generation_request_validate(&changed, NULL, 0U) != UMI_STATUS_OK);
    strcpy(plan->project_root, "/tmp/Umicom Notes"); strcpy(plan->template_id, "test");
    plan->file_count = 1U; strcpy(plan->files[0].relative_path, "main.c");
    strcpy(plan->files[0].content, "int main(void){return 0;}\n");
    CHECK(umi_developer_project_generation_plan_validate(plan) == UMI_STATUS_OK);
    strcpy(plan->files[0].relative_path, "C:outside.c");
    CHECK(umi_developer_project_generation_plan_validate(plan) == UMI_STATUS_PERMISSION_DENIED);
    strcpy(plan->files[0].relative_path, "main.c");
    memset(plan->files[0].content, 'X', sizeof(plan->files[0].content));
    CHECK(umi_developer_project_generation_plan_validate(plan) != UMI_STATUS_OK);
    plan->files[0].content[0] = '\0';
    memset(plan->files[0].relative_path, 'X', sizeof(plan->files[0].relative_path));
    CHECK(umi_developer_project_generation_plan_validate(plan) != UMI_STATUS_OK);
    umi_build_profile_init(&profile);
    unchanged = profile;
    CHECK(UmiBuildProfileForWorkspace(NULL, &profile) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&profile, &unchanged, sizeof(profile)) == 0);
    CHECK(UmiBuildProfileForWorkspace(".", &profile) == UMI_STATUS_OK);
    CHECK(umi_fs_is_absolute(profile.source_directory));
    CHECK(profile.run_program[0] == '\0');
    unchanged = profile; memset(profile.preset, 'X', sizeof(profile.preset));
    CHECK(umi_build_profile_validate(&profile, NULL, 0U) != UMI_STATUS_OK);
    CHECK(!umi_build_profile_equal(&profile, &unchanged));
    free(plan);
    puts("Project input boundaries passed.");
    return EXIT_SUCCESS;
}
