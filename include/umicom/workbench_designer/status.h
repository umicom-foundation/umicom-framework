/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/status.h
 *
 * PURPOSE:
 *   Project current mode, tool, pointer, zoom, selection, save and collaboration
 *   state into one status model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_STATUS_H
#define UMICOM_WORKBENCH_DESIGNER_STATUS_H

#include "umicom/workbench_designer/collaboration.h"
#include "umicom/workbench_designer/selection.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer status model data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerStatusModel {
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char layout_name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerMode mode;
    UmiWorkbenchDesignerTool tool;
    UmiWorkbenchDesignerSaveState save_state;
    UmiWorkbenchDesignerPoint pointer_world;
    double zoom;
    size_t selection_count;
    size_t collaborator_count;
    size_t editing_count;
    size_t issue_count;
    bool layout_locked;
    uint64_t document_revision;
    uint64_t revision;
} UmiWorkbenchDesignerStatusModel;

/**
 * Initialise workbench designer status from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_status_init(UmiWorkbenchDesignerStatusModel *status);
/**
 * Provide the workbench designer status update operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_status_update(UmiWorkbenchDesignerStatusModel *status, const char *message, const char *layout_name, UmiWorkbenchDesignerMode mode, UmiWorkbenchDesignerTool tool, UmiWorkbenchDesignerSaveState save_state, UmiWorkbenchDesignerPoint pointer_world, double zoom, const UmiWorkbenchDesignerSelection *selection, const UmiWorkbenchDesignerCollaborationModel *collaboration, size_t issue_count, bool layout_locked, uint64_t document_revision);

#ifdef __cplusplus
}
#endif

#endif
