/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_graph.c
 *
 * PURPOSE:
 *   Implement thread-safe workspace identity, project discovery, trust state
 *   and longest-root project resolution without GUI or product dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the make stable id operation used by this module and its client applications. */
static void make_stable_id(const char *name,
                           char *out_id,
                           size_t capacity)
{
    size_t read_index;
    size_t write_index = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (read_index = 0U; name[read_index] != '\0' &&
         write_index + 1U < capacity; ++read_index) {
        char value = name[read_index];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((value >= 'A' && value <= 'Z')) value = (char)(value - 'A' + 'a');
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((value >= 'a' && value <= 'z') ||
            (value >= '0' && value <= '9')) {
            out_id[write_index++] = value;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (write_index > 0U && out_id[write_index - 1U] != '-') {
            out_id[write_index++] = '-';
        }
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (write_index > 0U && out_id[write_index - 1U] == '-') {
        --write_index;
    }
    out_id[write_index] = '\0';
}

/*
 * Initialise workspace graph from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_workspace_graph_create(UmiWorkspaceGraph **out_graph)
{
    UmiWorkspaceGraph *graph;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_graph = NULL;
    graph = (UmiWorkspaceGraph *)calloc(1U, sizeof(*graph));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_mutex_create(&graph->mutex) != UMI_STATUS_OK) {
        free(graph);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    graph->next_project_id = 1U;
    graph->revision = 1U;
    *out_graph = graph;
    return UMI_STATUS_OK;
}

/* Release or reset state held by workspace graph so the same storage can be reused safely. */
void umi_workspace_graph_destroy(UmiWorkspaceGraph *graph)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return;
    umi_mutex_destroy(graph->mutex);
    free(graph);
}

/*
 * Provide the workspace graph open operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_graph_open(UmiWorkspaceGraph *graph,
                                   const char *root,
                                   int trusted)
{
    char absolute[UMI_PATH_CAPACITY];
    char current[UMI_PATH_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_current_directory(current, sizeof(current));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_path_absolute(root, current, absolute, sizeof(absolute));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the workspace graph close operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_graph_close(UmiWorkspaceGraph *graph)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the workspace graph set trusted operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_graph_set_trusted(UmiWorkspaceGraph *graph,
                                          int trusted)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(graph->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!graph->open) {
        (void)umi_mutex_unlock(graph->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    graph->trusted = trusted != 0;
    graph->revision += 1U;
    (void)umi_mutex_unlock(graph->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the workspace graph add project operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || display_name == NULL || root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(graph->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!graph->open) {
        (void)umi_mutex_unlock(graph->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_path_absolute(root,
                               graph->root,
                               absolute,
                               sizeof(absolute));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || !umi_path_is_within(graph->root, absolute)) {
        (void)umi_mutex_unlock(graph->mutex);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_PERMISSION_DENIED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->project_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_path_equal(graph->projects[index].root, absolute)) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (out_project_id != NULL) {
                *out_project_id = graph->projects[index].project_id;
            }
            (void)umi_mutex_unlock(graph->mutex);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stable_id != NULL && stable_id[0] != '\0') {
        (void)snprintf(project->stable_id,
                       sizeof(project->stable_id),
                       "%s",
                       stable_id);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        make_stable_id(display_name,
                       project->stable_id,
                       sizeof(project->stable_id));
    }
    graph->revision += 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_project_id != NULL) *out_project_id = project->project_id;
    (void)umi_mutex_unlock(graph->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the workspace graph remove project operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_graph_remove_project(UmiWorkspaceGraph *graph,
                                             uint64_t project_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || project_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(graph->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->project_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (graph->projects[index].project_id == project_id) {
            size_t remaining = graph->project_count - index - 1U;
            /* Apply this branch only when its contract condition is satisfied. */
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

/* Provide the discover visitor operation used by this module and its client applications. */
static UmiStatus discover_visitor(const UmiFileInfo *info, void *user_data)
{
    DiscoveryContext *context = (DiscoveryContext *)user_data;
    char cmake[UMI_PATH_CAPACITY];
    char marker[UMI_PATH_CAPACITY];
    char manifest[UMI_PATH_CAPACITY];
    UmiProjectKind kind = UMI_PROJECT_GENERIC;
    char name[UMI_WORKSPACE_NAME_CAPACITY];
    /* Apply this branch only when its contract condition is satisfied. */
    if (info->kind != UMI_FILE_KIND_DIRECTORY || info->depth > 0U) {
        return UMI_STATUS_OK;
    }
    (void)umi_path_join(info->path, "CMakeLists.txt", cmake, sizeof(cmake));
    (void)umi_path_join(info->path, ".umicom-root", marker, sizeof(marker));
    (void)umi_path_join(info->path,
                        "application.umicom.yaml",
                        manifest,
                        sizeof(manifest));
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_fs_is_file(marker) || umi_fs_is_file(manifest)) {
        kind = UMI_PROJECT_UMICOM;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (umi_fs_is_file(cmake)) {
        kind = UMI_PROJECT_CMAKE;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
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

/*
 * Provide the workspace graph discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_graph_discover(UmiWorkspaceGraph *graph)
{
    UmiDirectoryWalkOptions options;
    DiscoveryContext context;
    UmiWorkspaceGraphSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workspace_graph_snapshot(graph, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the workspace graph snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_graph_snapshot(const UmiWorkspaceGraph *graph,
                                        UmiWorkspaceGraphSnapshot *out_snapshot)
{
    UmiWorkspaceGraph *mutable_graph;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Find workspace graph project while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_workspace_graph_project_at(
    const UmiWorkspaceGraph *graph,
    size_t index,
    UmiWorkspaceProjectSnapshot *out_project)
{
    UmiWorkspaceGraph *mutable_graph;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || out_project == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    mutable_graph = (UmiWorkspaceGraph *)graph;
    (void)umi_mutex_lock(mutable_graph->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= graph->project_count) {
        (void)umi_mutex_unlock(mutable_graph->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_project = graph->projects[index];
    (void)umi_mutex_unlock(mutable_graph->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the workspace graph find project operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_graph_find_project(
    const UmiWorkspaceGraph *graph,
    const char *path,
    UmiWorkspaceProjectSnapshot *out_project)
{
    UmiWorkspaceGraph *mutable_graph;
    size_t index;
    size_t best_length = 0U;
    int found = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || path == NULL || out_project == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_graph = (UmiWorkspaceGraph *)graph;
    (void)umi_mutex_lock(mutable_graph->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->project_count; ++index) {
        size_t length = strlen(graph->projects[index].root);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
