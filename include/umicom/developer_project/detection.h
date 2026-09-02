/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/detection.h
 *
 * PURPOSE:
 *   Infer build system and language composition from an indexed workspace
 *   without mutating the project.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_DETECTION_H
#define UMICOM_DEVELOPER_PROJECT_DETECTION_H

#include "umicom/developer_project/model.h"
#include "umicom/developer_project/language_registry.h"
#include "umicom/platform/file_index.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project detection report data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectDetectionReport {
    UmiDeveloperProjectBuildSystem build_system;
    char primary_language_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    char language_ids[UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY]
                     [UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    size_t language_count;
    size_t source_file_count;
    int has_tests;
    int has_cmake_presets;
    int has_git_repository;
    uint64_t source_revision;
} UmiDeveloperProjectDetectionReport;

/**
 * Provide the developer project detect operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_project_detect(
    const UmiFileIndex *file_index,
    const UmiDeveloperProjectLanguageRegistry *languages,
    UmiDeveloperProjectDetectionReport *out_report);

/**
 * Provide the developer project detection to model operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_detection_to_model(
    const UmiDeveloperProjectDetectionReport *report,
    const char *project_id,
    const char *display_name,
    const char *root,
    UmiDeveloperProjectModel *out_model);

#ifdef __cplusplus
}
#endif

#endif
