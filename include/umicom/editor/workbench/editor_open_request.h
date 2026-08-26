/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_open_request.h
 *
 * PURPOSE:
 *   Represent a governed request to open a resource in an editor group.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_OPEN_REQUEST_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_OPEN_REQUEST_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorOpenRequest { char resource[UMI_EDITOR_WB_PATH_CAPACITY]; char group_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOpenMode mode; bool force; } UmiEditorWbEditorOpenRequest;
UmiStatus umi_editor_wb_editor_open_request_init(UmiEditorWbEditorOpenRequest *state,const char *resource,const char *group_id);
int umi_editor_wb_editor_open_request_valid(const UmiEditorWbEditorOpenRequest *state);

#ifdef __cplusplus
}
#endif
#endif
