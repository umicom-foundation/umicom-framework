/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/designer_workspace.h
 *
 * PURPOSE:
 *   Represent the complete visual designer workspace selection and dirty state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DESIGNER_WORKSPACE_H
#define UMICOM_DESIGNER_RAD_DESIGNER_WORKSPACE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDesignerWorkspace {
    char workspace_id[UMI_RAD_ID_CAPACITY];
    char document_id[UMI_RAD_ID_CAPACITY];
    char active_surface_id[UMI_RAD_ID_CAPACITY];
    size_t selection_count;
    bool dirty;
} UmiRadDesignerWorkspace;
UmiStatus umi_rad_designer_workspace_init(UmiRadDesignerWorkspace *item);
int umi_rad_designer_workspace_is_valid(const UmiRadDesignerWorkspace *item);
#ifdef __cplusplus
}
#endif
#endif
