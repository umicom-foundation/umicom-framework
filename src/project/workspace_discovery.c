/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_discovery.c
 * PURPOSE: Implement bounded CMake/build-directory workspace discovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace_discovery.h"

#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/project/workspace_exclusion.h"

#include <string.h>

typedef struct DiscoveryContext {
    const UmiProjectWorkspaceModel *model;
    const UmiProjectWorkspaceRootSnapshot *root;
    UmiProjectWorkspaceDiscoverySnapshot *snapshot;
    UmiProjectWorkspaceDiscoveryOptions options;
    size_t limit;
} DiscoveryContext;

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

static int file_in_directory(const char *directory, const char *name,
                             char *out_path, size_t capacity)
{
    return umi_path_join(directory, name, out_path, capacity) == UMI_STATUS_OK &&
           umi_fs_is_file(out_path);
}

static int directory_in_directory(const char *directory, const char *name,
                                  char *out_path, size_t capacity)
{
    return umi_path_join(directory, name, out_path, capacity) == UMI_STATUS_OK &&
           umi_fs_is_directory(out_path);
}

static void detect_build_directory(
    UmiProjectWorkspaceDiscoveredProject *project)
{
    static const char *const candidates[] = {
        "build", "out", "cmake-build-debug", "cmake-build-release"
    };
    size_t index;
    char candidate[UMI_PATH_CAPACITY];
    char compile_commands[UMI_PATH_CAPACITY];
    for (index = 0U; index < sizeof(candidates) / sizeof(candidates[0]); ++index) {
        if (!directory_in_directory(project->project_directory, candidates[index],
                                    candidate, sizeof(candidate))) continue;
        copy_text(project->build_directory, sizeof(project->build_directory),
                  candidate);
        project->has_build_directory = 1;
        if (file_in_directory(candidate, "compile_commands.json", compile_commands,
                              sizeof(compile_commands))) {
            copy_text(project->compile_commands_file,
                      sizeof(project->compile_commands_file), compile_commands);
            project->has_compile_commands = 1;
        }
        return;
    }
}

static int has_discovered_ancestor(const DiscoveryContext *context,
                                   const char *directory)
{
    size_t index;
    for (index = 0U; index < context->snapshot->project_count; ++index) {
        const UmiProjectWorkspaceDiscoveredProject *candidate =
            &context->snapshot->projects[index];
        if (strcmp(candidate->root_id, context->root->id) == 0 &&
            umi_path_is_within(candidate->project_directory, directory) &&
            !umi_path_equal(candidate->project_directory, directory))
            return 1;
    }
    return 0;
}

static UmiStatus discovery_visitor(const UmiFileInfo *info, void *user_data)
{
    DiscoveryContext *context = (DiscoveryContext *)user_data;
    UmiProjectWorkspaceDiscoveredProject *project;
    char relative[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];
    char presets_file[UMI_PATH_CAPACITY];
    char git_directory[UMI_PATH_CAPACITY];
    if (info == NULL || context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (info->kind != UMI_FILE_KIND_DIRECTORY) return UMI_STATUS_OK;
    context->snapshot->visited_directory_count += 1U;
    if (umi_path_relative(context->root->path, info->path, relative,
                          sizeof(relative)) != UMI_STATUS_OK)
        copy_text(relative, sizeof(relative), info->name);
    if (umi_project_workspace_model_path_excluded(
            context->model, relative, 1)) {
        context->snapshot->excluded_directory_count += 1U;
        return UMI_STATUS_OK;
    }
    if (!file_in_directory(info->path, "CMakeLists.txt", cmake_file,
                           sizeof(cmake_file))) return UMI_STATUS_OK;
    if (!context->options.include_nested_projects &&
        has_discovered_ancestor(context, info->path))
        return UMI_STATUS_OK;
    if (context->snapshot->project_count >= context->limit ||
        context->snapshot->project_count >= UMI_PROJECT_WORKSPACE_DISCOVERY_CAPACITY) {
        context->snapshot->truncated_project_count += 1U;
        return UMI_STATUS_OK;
    }
    project = &context->snapshot->projects[context->snapshot->project_count++];
    memset(project, 0, sizeof(*project));
    project->struct_size = (uint32_t)sizeof(*project);
    project->api_version = UMI_PROJECT_WORKSPACE_DISCOVERY_API_VERSION;
    copy_text(project->root_id, sizeof(project->root_id), context->root->id);
    copy_text(project->root_directory, sizeof(project->root_directory),
              context->root->path);
    copy_text(project->project_directory, sizeof(project->project_directory),
              info->path);
    copy_text(project->cmake_file, sizeof(project->cmake_file), cmake_file);
    project->has_cmake = 1;
    if (file_in_directory(info->path, "CMakePresets.json", presets_file,
                          sizeof(presets_file))) {
        copy_text(project->presets_file, sizeof(project->presets_file),
                  presets_file);
        project->has_presets = 1;
    }
    if (context->options.detect_git_repositories &&
        directory_in_directory(info->path, ".git", git_directory,
                               sizeof(git_directory)))
        project->has_git = 1;
    if (context->options.detect_build_directories)
        detect_build_directory(project);
    return UMI_STATUS_OK;
}

UmiProjectWorkspaceDiscoveryOptions
umi_project_workspace_discovery_options_default(void)
{
    UmiProjectWorkspaceDiscoveryOptions options;
    memset(&options, 0, sizeof(options));
    options.struct_size = (uint32_t)sizeof(options);
    options.api_version = UMI_PROJECT_WORKSPACE_DISCOVERY_API_VERSION;
    options.max_depth = 4U;
    options.max_projects = UMI_PROJECT_WORKSPACE_DISCOVERY_CAPACITY;
    options.include_nested_projects = 1;
    options.detect_build_directories = 1;
    options.detect_git_repositories = 1;
    return options;
}

UmiStatus umi_project_workspace_model_discover(
    const UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceDiscoveryOptions *options,
    UmiProjectWorkspaceDiscoverySnapshot *out_snapshot)
{
    UmiProjectWorkspaceDiscoveryOptions resolved;
    UmiDirectoryWalkOptions walk;
    size_t root_index;
    if (model == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    resolved = options != NULL ? *options
                               : umi_project_workspace_discovery_options_default();
    if (resolved.max_projects == 0U) resolved.max_projects = 1U;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_PROJECT_WORKSPACE_DISCOVERY_API_VERSION;
    walk = umi_directory_walk_options_default();
    walk.recursive = 1;
    walk.include_files = 0;
    walk.include_directories = 1;
    walk.include_hidden = 1; /* Rules still decide which hidden paths matter. */
    walk.follow_symbolic_links = 0;
    walk.max_depth = resolved.max_depth;
    for (root_index = 0U;
         root_index < umi_project_workspace_model_root_count(model);
         ++root_index) {
        UmiProjectWorkspaceRootSnapshot root;
        DiscoveryContext context;
        UmiStatus status;
        if (umi_project_workspace_model_root_at(model, root_index, &root) !=
            UMI_STATUS_OK || root.enabled == 0) continue;
        if (!umi_fs_is_directory(root.path)) return UMI_STATUS_NOT_FOUND;
        context.model = model;
        context.root = &root;
        context.snapshot = out_snapshot;
        context.options = resolved;
        context.limit = resolved.max_projects;
        {
            UmiFileInfo root_info;
            UmiStatus root_status = umi_directory_stat(root.path, &root_info);
            if (root_status != UMI_STATUS_OK) return root_status;
            root_info.depth = 0U;
            status = discovery_visitor(&root_info, &context);
            if (status != UMI_STATUS_OK) return status;
        }
        status = umi_directory_walk(root.path, &walk, discovery_visitor, &context);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
