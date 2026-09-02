/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/snapshot.h
 *
 * PURPOSE:
 *   Capture immutable Layout Designer service state for UI, diagnostics, tests
 *   and remote inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SNAPSHOT_H
#define UMICOM_WORKBENCH_DESIGNER_SNAPSHOT_H

#include "umicom/workbench_designer/controller.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerSnapshot {
    UmiWorkbenchDesignerState controller_state;
    size_t session_count;
    size_t dirty_session_count;
    char active_session_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char active_layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char active_layout_name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerMode mode;
    UmiWorkbenchDesignerTool tool;
    UmiWorkbenchDesignerSaveState save_state;
    size_t selection_count;
    size_t issue_count;
    size_t collaborator_count;
    uint64_t document_revision;
    uint64_t service_revision;
    uint64_t controller_revision;
} UmiWorkbenchDesignerSnapshot;

/**
 * Provide the workbench designer snapshot capture operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_snapshot_capture(const UmiWorkbenchDesignerController *controller, UmiWorkbenchDesignerSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
