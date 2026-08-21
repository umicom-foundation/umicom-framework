/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/session.h
 *
 * PURPOSE:
 *   Capture active layout, focus, open panels, selected workbench state and restore evidence separately from immutable layout templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_SESSION_H
#define UMICOM_WORKBENCH_LAYOUT_SESSION_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchOpenPanel {
    uint32_t structure_size;
    char instance_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char context_group_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char state_reference[UMI_WORKBENCH_LAYOUT_PATH_CAPACITY];
    bool visible;
    bool active;
    bool dirty;
    uint64_t revision;
} UmiWorkbenchOpenPanel;

typedef struct UmiWorkbenchLayoutSession {
    uint32_t structure_size;
    char session_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char active_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char active_layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char active_node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char active_panel_instance_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchOpenPanel
        panels[UMI_WORKBENCH_LAYOUT_MAX_OPEN_PANELS];
    size_t panel_count;
    uint64_t started_at_ms;
    uint64_t last_checkpoint_at_ms;
    uint64_t revision;
    bool clean_shutdown;
    bool recovery_available;
} UmiWorkbenchLayoutSession;

void umi_workbench_layout_session_init(
    UmiWorkbenchLayoutSession *session,
    const char *session_id,
    const char *user_id,
    const char *workspace_id);

UmiStatus umi_workbench_layout_session_validate(
    const UmiWorkbenchLayoutSession *session);

UmiStatus umi_workbench_layout_session_set_active_layout(
    UmiWorkbenchLayoutSession *session,
    const char *layout_id);

UmiStatus umi_workbench_layout_session_set_active_application(
    UmiWorkbenchLayoutSession *session,
    const char *application_id);

UmiStatus umi_workbench_layout_session_set_focus(
    UmiWorkbenchLayoutSession *session,
    const char *node_id,
    const char *panel_instance_id);

UmiStatus umi_workbench_layout_session_open_panel(
    UmiWorkbenchLayoutSession *session,
    const UmiWorkbenchOpenPanel *panel);

UmiStatus umi_workbench_layout_session_close_panel(
    UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id);

UmiStatus umi_workbench_layout_session_update_panel(
    UmiWorkbenchLayoutSession *session,
    const UmiWorkbenchOpenPanel *panel);

const UmiWorkbenchOpenPanel *umi_workbench_layout_session_find_panel(
    const UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id);

UmiWorkbenchOpenPanel *umi_workbench_layout_session_find_panel_mutable(
    UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id);

size_t umi_workbench_layout_session_visible_panel_count(
    const UmiWorkbenchLayoutSession *session);

size_t umi_workbench_layout_session_dirty_panel_count(
    const UmiWorkbenchLayoutSession *session);

UmiStatus umi_workbench_layout_session_mark_checkpoint(
    UmiWorkbenchLayoutSession *session,
    uint64_t checkpoint_at_ms);

UmiStatus umi_workbench_layout_session_mark_clean_shutdown(
    UmiWorkbenchLayoutSession *session,
    uint64_t shutdown_at_ms);

#ifdef __cplusplus
}
#endif

#endif
