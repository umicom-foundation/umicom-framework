/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/template.h
 *
 * PURPOSE:
 *   Define reusable project templates as bounded file recipes with variables.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_H

#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_TEMPLATE_FILE_CAPACITY 24U

typedef struct UmiDeveloperProjectTemplateFile {
    const char *relative_path;
    const char *content_template;
    int executable;
    int optional;
} UmiDeveloperProjectTemplateFile;

typedef struct UmiDeveloperProjectTemplate {
    uint32_t structure_size;
    uint32_t api_version;
    const char *template_id;
    const char *title;
    const char *description;
    UmiDeveloperProjectKind kind;
    UmiDeveloperProjectBuildSystem build_system;
    const char *primary_language_id;
    const char *const *secondary_language_ids;
    size_t secondary_language_count;
    unsigned recommended_frontends;
    const UmiDeveloperProjectTemplateFile *files;
    size_t file_count;
    int thin_application;
    int supports_tests;
    int supports_install;
    int supports_package;
} UmiDeveloperProjectTemplate;

UmiStatus umi_developer_project_template_validate(
    const UmiDeveloperProjectTemplate *project_template);

#ifdef __cplusplus
}
#endif

#endif
