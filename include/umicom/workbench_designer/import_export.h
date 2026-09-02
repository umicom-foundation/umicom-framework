/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/import_export.h
 *
 * PURPOSE:
 *   Prepare portable .umilayout import and export plans before filesystem side
 *   effects are performed by a platform adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_IMPORT_EXPORT_H
#define UMICOM_WORKBENCH_DESIGNER_IMPORT_EXPORT_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer transfer kind values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerTransferKind {
    UMI_WORKBENCH_DESIGNER_TRANSFER_IMPORT = 1,
    UMI_WORKBENCH_DESIGNER_TRANSFER_EXPORT = 2
} UmiWorkbenchDesignerTransferKind;

/**
 * Represent the workbench designer transfer plan data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerTransferPlan {
    UmiWorkbenchDesignerTransferKind kind;
    char operation_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char path[UMI_WORKBENCH_DESIGNER_PATH_CAPACITY];
    char actor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool overwrite_existing;
    bool preserve_audit;
    bool preserve_runtime_geometry;
    bool validate_schema;
} UmiWorkbenchDesignerTransferPlan;

/**
 * Initialise workbench designer transfer plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_transfer_plan_init(UmiWorkbenchDesignerTransferPlan *plan, UmiWorkbenchDesignerTransferKind kind, const char *operation_id);
/**
 * Check that workbench designer transfer plan satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_designer_transfer_plan_validate(const UmiWorkbenchDesignerTransferPlan *plan);
/**
 * Provide the workbench designer path is layout operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_path_is_layout(const char *path);

#ifdef __cplusplus
}
#endif

#endif
