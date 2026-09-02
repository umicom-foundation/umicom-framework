/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/session.h
 *
 * PURPOSE:
 *   Capture active layout, focus, open panels, selected workbench state and restore evidence separately from immutable layout templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_SESSION_H
#define UMICOM_WORKBENCH_LAYOUT_SESSION_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench open panel data shared with callers of this public contract.
 */
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

/**
 * Represent the workbench layout session data shared with callers of this public contract.
 */
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

/**
 * Initialise workbench layout session from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_session_init(
    UmiWorkbenchLayoutSession *session,
    const char *session_id,
    const char *user_id,
    const char *workspace_id);

/**
 * Check that workbench layout session satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_workbench_layout_session_validate(
    const UmiWorkbenchLayoutSession *session);

/**
 * Provide the workbench layout session set active layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_session_set_active_layout(
    UmiWorkbenchLayoutSession *session,
    const char *layout_id);

/**
 * Provide the workbench layout session set active application operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_layout_session_set_active_application(
    UmiWorkbenchLayoutSession *session,
    const char *application_id);

/**
 * Provide the workbench layout session set focus operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_session_set_focus(
    UmiWorkbenchLayoutSession *session,
    const char *node_id,
    const char *panel_instance_id);

/**
 * Provide the workbench layout session open panel operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_session_open_panel(
    UmiWorkbenchLayoutSession *session,
    const UmiWorkbenchOpenPanel *panel);

/**
 * Provide the workbench layout session close panel operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_session_close_panel(
    UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id);

/**
 * Provide the workbench layout session update panel operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_session_update_panel(
    UmiWorkbenchLayoutSession *session,
    const UmiWorkbenchOpenPanel *panel);

/**
 * Provide the workbench layout session find panel operation used by this module and its
 * client applications.
 */
const UmiWorkbenchOpenPanel *umi_workbench_layout_session_find_panel(
    const UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id);

/**
 * Provide the workbench layout session find panel mutable operation used by this module
 * and its client applications.
 */
UmiWorkbenchOpenPanel *umi_workbench_layout_session_find_panel_mutable(
    UmiWorkbenchLayoutSession *session,
    const char *panel_instance_id);

/**
 * Return the number of records represented by workbench layout session visible panel
 * without changing their state.
 */
size_t umi_workbench_layout_session_visible_panel_count(
    const UmiWorkbenchLayoutSession *session);

/**
 * Return the number of records represented by workbench layout session dirty panel without
 * changing their state.
 */
size_t umi_workbench_layout_session_dirty_panel_count(
    const UmiWorkbenchLayoutSession *session);

/**
 * Provide the workbench layout session mark checkpoint operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_session_mark_checkpoint(
    UmiWorkbenchLayoutSession *session,
    uint64_t checkpoint_at_ms);

/**
 * Provide the workbench layout session mark clean shutdown operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_session_mark_clean_shutdown(
    UmiWorkbenchLayoutSession *session,
    uint64_t shutdown_at_ms);

#ifdef __cplusplus
}
#endif

#endif
