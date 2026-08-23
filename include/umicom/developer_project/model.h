/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/model.h
 *
 * PURPOSE:
 *   Represent one IDE-visible project independently from CMake, GTK and Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_MODEL_H
#define UMICOM_DEVELOPER_PROJECT_MODEL_H

#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectModel {
    uint32_t structure_size;
    uint32_t api_version;
    char project_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char display_name[UMI_DEVELOPER_PROJECT_NAME_CAPACITY];
    char root[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char template_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char primary_language_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char secondary_language_ids[UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY]
                               [UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    size_t secondary_language_count;
    char entry_point[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char preset[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char build_directory[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char executable[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    char install_prefix[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    UmiDeveloperProjectKind kind;
    UmiDeveloperProjectBuildSystem build_system;
    int generated;
    int trusted;
    int dirty_configuration;
    uint64_t revision;
} UmiDeveloperProjectModel;

void umi_developer_project_model_init(
    UmiDeveloperProjectModel *model,
    const char *project_id,
    const char *display_name);

UmiStatus umi_developer_project_model_validate(
    const UmiDeveloperProjectModel *model,
    char *out_message,
    size_t message_capacity);

UmiStatus umi_developer_project_model_add_language(
    UmiDeveloperProjectModel *model,
    const char *language_id);

#ifdef __cplusplus
}
#endif

#endif
