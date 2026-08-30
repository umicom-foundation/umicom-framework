/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/generation_plan.h
 *
 * PURPOSE:
 *   Render a project template into a deterministic file plan before mutating the
 *   filesystem. The plan can be previewed in Studio or reviewed by an AI agent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_GENERATION_PLAN_H
#define UMICOM_DEVELOPER_PROJECT_GENERATION_PLAN_H

#include "umicom/developer_project/render.h"
#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_GENERATED_CONTENT_CAPACITY 16384U
#define UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY 32U

typedef struct UmiDeveloperProjectGeneratedFile {
    char relative_path[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char content[UMI_DEVELOPER_PROJECT_GENERATED_CONTENT_CAPACITY];
    int executable;
    int optional;
} UmiDeveloperProjectGeneratedFile;

typedef struct UmiDeveloperProjectGenerationPlan {
    char project_root[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char template_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    UmiDeveloperProjectGeneratedFile
        files[UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY];
    size_t file_count;
    size_t directory_count;
    size_t total_content_bytes;
    uint64_t revision;
} UmiDeveloperProjectGenerationPlan;

void umi_developer_project_generation_plan_init(
    UmiDeveloperProjectGenerationPlan *plan);

UmiStatus umi_developer_project_generation_plan_build(
    const UmiDeveloperProjectTemplate *project_template,
    const UmiDeveloperProjectVariableSet *variables,
    const char *project_root,
    UmiDeveloperProjectGenerationPlan *out_plan);

UmiStatus umi_developer_project_generation_plan_validate(
    const UmiDeveloperProjectGenerationPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
