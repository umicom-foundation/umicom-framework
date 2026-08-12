/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_discovery.h
 * PURPOSE: Discover CMake projects and build directories in multi-root workspaces.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_DISCOVERY_H
#define UMICOM_PROJECT_WORKSPACE_DISCOVERY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/project/workspace_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_DISCOVERY_API_VERSION 1U
/*
 * Discovery snapshots contain complete portable paths and are commonly local
 * variables.  Sixty-four projects keeps the bounded value comfortably below
 * the default Windows thread stack while still covering large workspaces. A
 * truncated count tells callers when to narrow roots or depth and retry.
 */
#define UMI_PROJECT_WORKSPACE_DISCOVERY_CAPACITY 64U

typedef struct UmiProjectWorkspaceDiscoveryOptions {
    uint32_t struct_size;
    uint32_t api_version;
    size_t max_depth;
    size_t max_projects;
    int include_nested_projects;
    int detect_build_directories;
    int detect_git_repositories;
} UmiProjectWorkspaceDiscoveryOptions;

typedef struct UmiProjectWorkspaceDiscoveredProject {
    uint32_t struct_size;
    uint32_t api_version;
    char root_id[128];
    char root_directory[2048];
    char project_directory[2048];
    char cmake_file[2048];
    char presets_file[2048];
    char build_directory[2048];
    char compile_commands_file[2048];
    int has_cmake;
    int has_presets;
    int has_build_directory;
    int has_compile_commands;
    int has_git;
} UmiProjectWorkspaceDiscoveredProject;

typedef struct UmiProjectWorkspaceDiscoverySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiProjectWorkspaceDiscoveredProject projects[
        UMI_PROJECT_WORKSPACE_DISCOVERY_CAPACITY];
    size_t project_count;
    size_t visited_directory_count;
    size_t excluded_directory_count;
    size_t truncated_project_count;
} UmiProjectWorkspaceDiscoverySnapshot;

UmiProjectWorkspaceDiscoveryOptions
umi_project_workspace_discovery_options_default(void);
UmiStatus umi_project_workspace_model_discover(
    const UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceDiscoveryOptions *options,
    UmiProjectWorkspaceDiscoverySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
