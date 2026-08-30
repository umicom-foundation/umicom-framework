/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_import.h
 *
 * PURPOSE:
 *   Import an existing filesystem directory into the reusable project workspace
 *   and create deterministic project, build, task, environment and launch
 *   records without exposing product-specific UI or private registry state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Importing a directory does not run CMake, Git, a compiler, or an executable.
 * It inspects the directory, records what is present, and creates safe project
 * metadata that the developer runtime can later validate and execute.
 */
#ifndef UMICOM_PROJECT_WORKSPACE_IMPORT_H
#define UMICOM_PROJECT_WORKSPACE_IMPORT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/project/workspace.h"
#include "umicom/project/workspace_query.h"
#include "umicom/project/workspace_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION 1U
#define UMI_PROJECT_WORKSPACE_IMPORT_DEFAULT_PARALLEL_JOBS 2U

typedef struct UmiProjectWorkspaceImportRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *root_directory;
    const char *project_id;
    const char *display_name;
    const char *build_directory;
    const char *configuration_name;
    const char *generator;
    const char *toolchain_id;
    const char *launch_program;
    const char *launch_arguments;
    uint32_t parallel_jobs;
    int create_test_task;
} UmiProjectWorkspaceImportRequest;

typedef struct UmiProjectWorkspaceImportSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char project_id[128];
    char display_name[256];
    char root_directory[2048];
    char build_directory[2048];
    char configuration_id[128];
    char target_id[128];
    char environment_id[128];
    char configure_task_id[128];
    char build_task_id[128];
    char test_task_id[128];
    char launch_profile_id[128];
    size_t discovered_file_count;
    size_t c_source_count;
    size_t cpp_source_count;
    size_t header_count;
    size_t created_or_updated_record_count;
    int has_cmake;
    int has_git;
    int has_launch_profile;
    int created_test_task;
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
} UmiProjectWorkspaceImportSnapshot;

UmiStatus umi_project_workspace_import_directory(
    UmiProjectWorkspace *workspace,
    const UmiProjectWorkspaceImportRequest *request,
    UmiProjectWorkspaceImportSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
