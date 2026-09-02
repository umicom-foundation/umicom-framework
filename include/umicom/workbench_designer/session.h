/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/session.h
 *
 * PURPOSE:
 *   Own one open layout design session, its semantic document, history,
 *   projections, autosave state and collaboration evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SESSION_H
#define UMICOM_WORKBENCH_DESIGNER_SESSION_H

#include "umicom/workbench_designer/autosave.h"
#include "umicom/workbench_designer/canvas.h"
#include "umicom/workbench_designer/clipboard.h"
#include "umicom/workbench_designer/collaboration.h"
#include "umicom/workbench_designer/command_history.h"
#include "umicom/workbench_designer/diagnostics.h"
#include "umicom/workbench_designer/docking_guides.h"
#include "umicom/workbench_designer/drop_target.h"
#include "umicom/workbench_designer/grid.h"
#include "umicom/workbench_designer/keymap.h"
#include "umicom/workbench_designer/lease_view.h"
#include "umicom/workbench_designer/monitor_canvas.h"
#include "umicom/workbench_designer/palette.h"
#include "umicom/workbench_designer/properties.h"
#include "umicom/workbench_designer/responsive_preview.h"
#include "umicom/workbench_designer/status.h"
#include "umicom/workbench_designer/toolbar.h"
#include "umicom/workbench_designer/tree.h"
#include "umicom/workbench_designer/workspace.h"
#include "umicom/workbench_layout/history.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer session data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerSession UmiWorkbenchDesignerSession;

/**
 * Represent the workbench designer session config data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerSessionConfig {
    UmiWorkbenchDesignerAutosavePolicy autosave_policy;
    double initial_canvas_width;
    double initial_canvas_height;
    UmiWorkbenchDesignerRect root_bounds;
    bool administrator_override;
} UmiWorkbenchDesignerSessionConfig;

/**
 * Provide the workbench designer session config default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerSessionConfig umi_workbench_designer_session_config_default(void);
/**
 * Initialise workbench designer session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_designer_session_create(const char *session_id, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSessionConfig *config, UmiWorkbenchDesignerSession **out_session);
/**
 * Release or reset state held by workbench designer session so the same storage can be
 * reused safely.
 */
void umi_workbench_designer_session_destroy(UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session rebuild operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_rebuild(UmiWorkbenchDesignerSession *session);
/**
 * Perform workbench designer session through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_designer_session_apply(UmiWorkbenchDesignerSession *session, const UmiWorkbenchDesignerCommand *command, UmiWorkbenchDesignerCommandResult *out_result);
/**
 * Provide the workbench designer session undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_session_undo(UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_session_redo(UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session set mode operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_set_mode(UmiWorkbenchDesignerSession *session, UmiWorkbenchDesignerMode mode);
/**
 * Provide the workbench designer session set tool operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_set_tool(UmiWorkbenchDesignerSession *session, UmiWorkbenchDesignerTool tool);
/**
 * Provide the workbench designer session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_select(UmiWorkbenchDesignerSession *session, const char *node_id, bool extend, bool toggle);
/**
 * Provide the workbench designer session clear selection operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_session_clear_selection(UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session id operation used by this module and its client
 * applications.
 */
const char *umi_workbench_designer_session_id(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session document operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document(UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session document const operation used by this module and
 * its client applications.
 */
const UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document_const(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session selection operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerSelection *umi_workbench_designer_session_selection(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session canvas operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerCanvas *umi_workbench_designer_session_canvas(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session viewport operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerViewport *umi_workbench_designer_session_viewport(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session palette operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerPalette *umi_workbench_designer_session_palette(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session collaboration operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerCollaborationModel *umi_workbench_designer_session_collaboration(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session drop targets operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDropTargetSet *umi_workbench_designer_session_drop_targets(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session docking guides operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDockingGuideModel *umi_workbench_designer_session_docking_guides(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session tree operation used by this module and its client
 * applications.
 */
const UmiWorkbenchDesignerTree *umi_workbench_designer_session_tree(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session properties operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerPropertyModel *umi_workbench_designer_session_properties(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session diagnostics operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerDiagnostics *umi_workbench_designer_session_diagnostics(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session status operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerStatusModel *umi_workbench_designer_session_status(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session autosave operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerAutosave *umi_workbench_designer_session_autosave(UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session mode operation used by this module and its client
 * applications.
 */
UmiWorkbenchDesignerMode umi_workbench_designer_session_mode(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session tool operation used by this module and its client
 * applications.
 */
UmiWorkbenchDesignerTool umi_workbench_designer_session_tool(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session dirty operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_session_dirty(const UmiWorkbenchDesignerSession *session);
/**
 * Provide the workbench designer session revision operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_designer_session_revision(const UmiWorkbenchDesignerSession *session);

#ifdef __cplusplus
}
#endif

#endif
