/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_graph.c
 *
 * PURPOSE:
 *   Implement thread-safe workspace identity, project discovery, trust state
 *   and longest-root project resolution without GUI or product dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/workspace_graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

struct UmiWorkspaceGraph {
    char root[UMI_PATH_CAPACITY];
    UmiWorkspaceProjectSnapshot projects[UMI_WORKSPACE_PROJECT_MAX];
    size_t project_count;
    uint64_t next_project_id;
    uint64_t revision;
    int trusted;
    int open;
    UmiMutex *mutex;
};

static void make_stable_id(const char *name,
                           char *out_id,
                           size_t capacity)
{
    size_t read_index;
    size_t write_index = 0U;
    for (read_index = 0U; name[read_index] != '\0' &&
         write_index + 1U < capacity; ++read_index) {
        char value = name[read_index];
        if ((value >= 'A' && value <= 'Z')) value = (char)(value - 'A' + 'a');
        if ((value >= 'a' && value <= 'z') ||
            (value >= '0' && value <= '9')) {
            out_id[write_index++] = value;
        } else if (write_index > 0U && out_id[write_index - 1U] != '-') {
            out_id[write_index++] = '-';
        }
    }
    while (write_index > 0U && out_id[write_index - 1U] == '-') {
        --write_index;
    }
    out_id[write_index] = '\0';
}

UmiStatus umi_workspace_graph_create(UmiWorkspaceGraph **out_graph)
{
    UmiWorkspaceGraph *graph;
    if (out_graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_graph = NULL;
    graph = (UmiWorkspaceGraph *)calloc(1U, sizeof(*graph));
    if (graph == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (umi_mutex_create(&graph->mutex) != UMI_STATUS_OK) {
        free(graph);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    graph->next_project_id = 1U;
    graph->revision = 1U;
    *out_graph = graph;
    return UMI_STATUS_OK;
}

void umi_workspace_graph_destroy(UmiWorkspaceGraph *graph)
{
    if (graph == NULL) return;
    umi_mutex_destroy(graph->mutex);
    free(graph);
}

UmiStatus umi_workspace_graph_open(UmiWorkspaceGraph *graph,
                                   const char *root,
                                   int trusted)
{
    char absolute[UMI_PATH_CAPACITY];
    char current[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (graph == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_current_directory(current, sizeof(current));
    if (status != UMI_STATUS_OK) return status;
    status = umi_path_absolute(root, current, absolute, sizeof(absolute));
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_directory(absolute)) return UMI_STATUS_NOT_FOUND;
    (void)umi_mutex_lock(graph->mutex);
    (void)snprintf(graph->root, sizeof(graph->root), "%s", absolute);
    graph->project_count = 0U;
    graph->trusted = trusted != 0;
    graph->open = 1;
    graph->revision += 1U;
    (void)umi_mutex_unlock(graph->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_workspace_graph_close(UmiWorkspaceGraph *graph)
{
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(graph->mutex);
    graph->root[0] = '\0';
    graph->project_count = 0U;
    graph->trusted = 0;
    graph->open = 0;
    graph->revision += 1U;
    (void)umi_mutex_unlock(graph->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_workspace_graph_set_trusted(UmiWorkspaceGraph *graph,
                                          int trusted)
{
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(graph->mutex);
    if (!graph->open) {
        (void)umi_mutex_unlock(graph->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    graph->trusted = trusted != 0;
    graph->revision += 1U;
    (void)umi_mutex_unlock(graph->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_workspace_graph_add_project(UmiWorkspaceGraph *graph,
                                          const char *stable_id,
                                          const char *display_name,
                                          const char *root,
                                          UmiProjectKind kind,
                                          uint64_t *out_project_id)
{
    UmiWorkspaceProjectSnapshot *project;
    char absolute[UMI_PATH_CAPACITY];
    size_t index;
    UmiStatus status;
    if (graph == NULL || display_name == NULL || root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(graph->mutex);
    if (!graph->open) {
        (void)umi_mutex_unlock(graph->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_path_absolute(root,
                               graph->root,
                               absolute,
                               sizeof(absolute));
    if (status != UMI_STATUS_OK || !umi_path_is_within(graph->root, absolute)) {
        (void)umi_mutex_unlock(graph->mutex);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_PERMISSION_DENIED;
    }
    for (index = 0U; index < graph->project_count; ++index) {
        if (umi_path_equal(graph->projects[index].root, absolute)) {
            if (out_project_id != NULL) {
                *out_project_id = graph->projects[index].project_id;
            }
            (void)umi_mutex_unlock(graph->mutex);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (graph->project_count >= UMI_WORKSPACE_PROJECT_MAX) {
        (void)umi_mutex_unlock(graph->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    project = &graph->projects[graph->project_count++];
    (void)memset(project, 0, sizeof(*project));
    project->project_id = graph->next_project_id++;
    project->kind = kind;
    (void)snprintf(project->display_name,
                   sizeof(project->display_name),
                   "%s",
                   display_name);
    (void)snprintf(project->root, sizeof(project->root), "%s", absolute);
    if (stable_id != NULL && stable_id[0] != '\0') {
        (void)snprintf(project->stable_id,
                       sizeof(project->stable_id),
                       "%s",
                       stable_id);
    } else {
        make_stable_id(display_name,
                       project->stable_id,
                       sizeof(project->stable_id));
    }
    graph->revision += 1U;
    if (out_project_id != NULL) *out_project_id = project->project_id;
    (void)umi_mutex_unlock(graph->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_workspace_graph_remove_project(UmiWorkspaceGraph *graph,
                                             uint64_t project_id)
{
    size_t index;
    if (graph == NULL || project_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(graph->mutex);
    for (index = 0U; index < graph->project_count; ++index) {
        if (graph->projects[index].project_id == project_id) {
            size_t remaining = graph->project_count - index - 1U;
            if (remaining > 0U) {
                (void)memmove(&graph->projects[index],
                              &graph->projects[index + 1U],
                              remaining * sizeof(graph->projects[0]));
            }
            graph->project_count -= 1U;
            graph->revision += 1U;
            (void)umi_mutex_unlock(graph->mutex);
            return UMI_STATUS_OK;
        }
    }
    (void)umi_mutex_unlock(graph->mutex);
    return UMI_STATUS_NOT_FOUND;
}

typedef struct DiscoveryContext {
    UmiWorkspaceGraph *graph;
} DiscoveryContext;

static UmiStatus discover_visitor(const UmiFileInfo *info, void *user_data)
{
    DiscoveryContext *context = (DiscoveryContext *)user_data;
    char cmake[UMI_PATH_CAPACITY];
    char marker[UMI_PATH_CAPACITY];
    char manifest[UMI_PATH_CAPACITY];
    UmiProjectKind kind = UMI_PROJECT_GENERIC;
    char name[UMI_WORKSPACE_NAME_CAPACITY];
    if (info->kind != UMI_FILE_KIND_DIRECTORY || info->depth > 0U) {
        return UMI_STATUS_OK;
    }
    (void)umi_path_join(info->path, "CMakeLists.txt", cmake, sizeof(cmake));
    (void)umi_path_join(info->path, ".umicom-root", marker, sizeof(marker));
    (void)umi_path_join(info->path,
                        "application.umicom.yaml",
                        manifest,
                        sizeof(manifest));
    if (umi_fs_is_file(marker) || umi_fs_is_file(manifest)) {
        kind = UMI_PROJECT_UMICOM;
    } else if (umi_fs_is_file(cmake)) {
        kind = UMI_PROJECT_CMAKE;
    } else {
        return UMI_STATUS_OK;
    }
    (void)umi_path_basename(info->path, name, sizeof(name));
    {
        UmiStatus status = umi_workspace_graph_add_project(context->graph,
                                                            NULL,
                                                            name,
                                                            info->path,
                                                            kind,
                                                            NULL);
        return status == UMI_STATUS_ALREADY_EXISTS ? UMI_STATUS_OK : status;
    }
}

UmiStatus umi_workspace_graph_discover(UmiWorkspaceGraph *graph)
{
    UmiDirectoryWalkOptions options;
    DiscoveryContext context;
    UmiWorkspaceGraphSnapshot snapshot;
    UmiStatus status;
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workspace_graph_snapshot(graph, &snapshot);
    if (status != UMI_STATUS_OK || !snapshot.open) {
        return UMI_STATUS_INVALID_STATE;
    }
    options = umi_directory_walk_options_default();
    options.recursive = 0;
    options.include_files = 0;
    options.include_directories = 1;
    context.graph = graph;
    return umi_directory_walk(snapshot.root,
                              &options,
                              discover_visitor,
                              &context);
}

UmiStatus umi_workspace_graph_snapshot(const UmiWorkspaceGraph *graph,
                                        UmiWorkspaceGraphSnapshot *out_snapshot)
{
    UmiWorkspaceGraph *mutable_graph;
    if (graph == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_graph = (UmiWorkspaceGraph *)graph;
    (void)umi_mutex_lock(mutable_graph->mutex);
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->root,
                   sizeof(out_snapshot->root),
                   "%s",
                   graph->root);
    out_snapshot->revision = graph->revision;
    out_snapshot->project_count = graph->project_count;
    out_snapshot->trusted = graph->trusted;
    out_snapshot->open = graph->open;
    (void)umi_mutex_unlock(mutable_graph->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_workspace_graph_project_at(
    const UmiWorkspaceGraph *graph,
    size_t index,
    UmiWorkspaceProjectSnapshot *out_project)
{
    UmiWorkspaceGraph *mutable_graph;
    if (graph == NULL || out_project == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    mutable_graph = (UmiWorkspaceGraph *)graph;
    (void)umi_mutex_lock(mutable_graph->mutex);
    if (index >= graph->project_count) {
        (void)umi_mutex_unlock(mutable_graph->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_project = graph->projects[index];
    (void)umi_mutex_unlock(mutable_graph->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_workspace_graph_find_project(
    const UmiWorkspaceGraph *graph,
    const char *path,
    UmiWorkspaceProjectSnapshot *out_project)
{
    UmiWorkspaceGraph *mutable_graph;
    size_t index;
    size_t best_length = 0U;
    int found = 0;
    if (graph == NULL || path == NULL || out_project == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_graph = (UmiWorkspaceGraph *)graph;
    (void)umi_mutex_lock(mutable_graph->mutex);
    for (index = 0U; index < graph->project_count; ++index) {
        size_t length = strlen(graph->projects[index].root);
        if (length > best_length &&
            umi_path_is_within(graph->projects[index].root, path)) {
            *out_project = graph->projects[index];
            best_length = length;
            found = 1;
        }
    }
    (void)umi_mutex_unlock(mutable_graph->mutex);
    return found ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
