/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/session.h
 *
 * PURPOSE:
 *   Own one open layout design session, its semantic document, history,
 *   projections, autosave state and collaboration evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiWorkbenchDesignerSession UmiWorkbenchDesignerSession;

typedef struct UmiWorkbenchDesignerSessionConfig {
    UmiWorkbenchDesignerAutosavePolicy autosave_policy;
    double initial_canvas_width;
    double initial_canvas_height;
    UmiWorkbenchDesignerRect root_bounds;
    bool administrator_override;
} UmiWorkbenchDesignerSessionConfig;

UmiWorkbenchDesignerSessionConfig umi_workbench_designer_session_config_default(void);
UmiStatus umi_workbench_designer_session_create(const char *session_id, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSessionConfig *config, UmiWorkbenchDesignerSession **out_session);
void umi_workbench_designer_session_destroy(UmiWorkbenchDesignerSession *session);
UmiStatus umi_workbench_designer_session_rebuild(UmiWorkbenchDesignerSession *session);
UmiStatus umi_workbench_designer_session_apply(UmiWorkbenchDesignerSession *session, const UmiWorkbenchDesignerCommand *command, UmiWorkbenchDesignerCommandResult *out_result);
UmiStatus umi_workbench_designer_session_undo(UmiWorkbenchDesignerSession *session);
UmiStatus umi_workbench_designer_session_redo(UmiWorkbenchDesignerSession *session);
UmiStatus umi_workbench_designer_session_set_mode(UmiWorkbenchDesignerSession *session, UmiWorkbenchDesignerMode mode);
UmiStatus umi_workbench_designer_session_set_tool(UmiWorkbenchDesignerSession *session, UmiWorkbenchDesignerTool tool);
UmiStatus umi_workbench_designer_session_select(UmiWorkbenchDesignerSession *session, const char *node_id, bool extend, bool toggle);
void umi_workbench_designer_session_clear_selection(UmiWorkbenchDesignerSession *session);
const char *umi_workbench_designer_session_id(const UmiWorkbenchDesignerSession *session);
UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document(UmiWorkbenchDesignerSession *session);
const UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document_const(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerSelection *umi_workbench_designer_session_selection(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerCanvas *umi_workbench_designer_session_canvas(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerViewport *umi_workbench_designer_session_viewport(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerPalette *umi_workbench_designer_session_palette(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerCollaborationModel *umi_workbench_designer_session_collaboration(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerDropTargetSet *umi_workbench_designer_session_drop_targets(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerDockingGuideModel *umi_workbench_designer_session_docking_guides(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerTree *umi_workbench_designer_session_tree(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerPropertyModel *umi_workbench_designer_session_properties(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerDiagnostics *umi_workbench_designer_session_diagnostics(const UmiWorkbenchDesignerSession *session);
const UmiWorkbenchDesignerStatusModel *umi_workbench_designer_session_status(const UmiWorkbenchDesignerSession *session);
UmiWorkbenchDesignerAutosave *umi_workbench_designer_session_autosave(UmiWorkbenchDesignerSession *session);
UmiWorkbenchDesignerMode umi_workbench_designer_session_mode(const UmiWorkbenchDesignerSession *session);
UmiWorkbenchDesignerTool umi_workbench_designer_session_tool(const UmiWorkbenchDesignerSession *session);
bool umi_workbench_designer_session_dirty(const UmiWorkbenchDesignerSession *session);
uint64_t umi_workbench_designer_session_revision(const UmiWorkbenchDesignerSession *session);

#ifdef __cplusplus
}
#endif

#endif
