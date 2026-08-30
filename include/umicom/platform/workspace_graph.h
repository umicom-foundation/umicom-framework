/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/workspace_graph.h
 *
 * PURPOSE:
 *   Define authoritative toolkit-neutral workspace and project ownership with
 *   stable identifiers, trust state, discovery and immutable snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_WORKSPACE_GRAPH_H
#define UMICOM_PLATFORM_WORKSPACE_GRAPH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKSPACE_PROJECT_MAX 128U
#define UMI_WORKSPACE_ID_CAPACITY 128U
#define UMI_WORKSPACE_NAME_CAPACITY 256U

typedef enum UmiProjectKind {
    UMI_PROJECT_GENERIC = 0,
    UMI_PROJECT_CMAKE = 1,
    UMI_PROJECT_UMICOM = 2
} UmiProjectKind;

typedef struct UmiWorkspaceProjectSnapshot {
    uint64_t project_id;
    char stable_id[UMI_WORKSPACE_ID_CAPACITY];
    char display_name[UMI_WORKSPACE_NAME_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    UmiProjectKind kind;
} UmiWorkspaceProjectSnapshot;

typedef struct UmiWorkspaceGraphSnapshot {
    char root[UMI_PATH_CAPACITY];
    uint64_t revision;
    size_t project_count;
    int trusted;
    int open;
} UmiWorkspaceGraphSnapshot;

typedef struct UmiWorkspaceGraph UmiWorkspaceGraph;

UmiStatus umi_workspace_graph_create(UmiWorkspaceGraph **out_graph);
void umi_workspace_graph_destroy(UmiWorkspaceGraph *graph);
UmiStatus umi_workspace_graph_open(UmiWorkspaceGraph *graph,
                                   const char *root,
                                   int trusted);
UmiStatus umi_workspace_graph_close(UmiWorkspaceGraph *graph);
UmiStatus umi_workspace_graph_set_trusted(UmiWorkspaceGraph *graph,
                                          int trusted);
UmiStatus umi_workspace_graph_add_project(UmiWorkspaceGraph *graph,
                                          const char *stable_id,
                                          const char *display_name,
                                          const char *root,
                                          UmiProjectKind kind,
                                          uint64_t *out_project_id);
UmiStatus umi_workspace_graph_remove_project(UmiWorkspaceGraph *graph,
                                             uint64_t project_id);
UmiStatus umi_workspace_graph_discover(UmiWorkspaceGraph *graph);
UmiStatus umi_workspace_graph_snapshot(const UmiWorkspaceGraph *graph,
                                        UmiWorkspaceGraphSnapshot *out_snapshot);
UmiStatus umi_workspace_graph_project_at(
    const UmiWorkspaceGraph *graph,
    size_t index,
    UmiWorkspaceProjectSnapshot *out_project
);
UmiStatus umi_workspace_graph_find_project(
    const UmiWorkspaceGraph *graph,
    const char *path,
    UmiWorkspaceProjectSnapshot *out_project
);

#ifdef __cplusplus
}
#endif

#endif
