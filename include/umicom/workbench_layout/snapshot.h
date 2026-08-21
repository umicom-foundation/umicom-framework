/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/snapshot.h
 *
 * PURPOSE:
 *   Project immutable layout and session state for GTK4, Qt, Wt, web and headless consumers without exposing internal storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_SNAPSHOT_H
#define UMICOM_WORKBENCH_LAYOUT_SNAPSHOT_H

#include "umicom/workbench_layout/document.h"
#include "umicom/workbench_layout/session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutNodeSnapshot {
    uint32_t structure_size;
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char title[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char component_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char context_group_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutNodeKind kind;
    UmiWorkbenchLayoutOrientation orientation;
    UmiWorkbenchLayoutDockRegion dock_region;
    UmiWorkbenchLayoutVisibility visibility;
    UmiWorkbenchLayoutRect bounds;
    double split_ratio;
    size_t depth;
    size_t child_count;
    bool active;
    bool focused;
    bool locked;
} UmiWorkbenchLayoutNodeSnapshot;

typedef struct UmiWorkbenchLayoutSnapshot {
    uint32_t structure_size;
    UmiWorkbenchLayoutIdentity identity;
    UmiWorkbenchLayoutVersion version;
    char name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char category[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutNodeSnapshot
        nodes[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t node_count;
    size_t visible_node_count;
    size_t panel_count;
    size_t window_count;
    size_t floating_window_count;
    char focused_node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    bool locked;
    bool dirty;
    uint64_t content_hash;
} UmiWorkbenchLayoutSnapshot;

UmiStatus umi_workbench_layout_snapshot_build(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutSession *session,
    UmiWorkbenchLayoutSnapshot *out_snapshot);

const UmiWorkbenchLayoutNodeSnapshot *
umi_workbench_layout_snapshot_find(
    const UmiWorkbenchLayoutSnapshot *snapshot,
    const char *node_id);

const UmiWorkbenchLayoutNodeSnapshot *
umi_workbench_layout_snapshot_at(
    const UmiWorkbenchLayoutSnapshot *snapshot,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
