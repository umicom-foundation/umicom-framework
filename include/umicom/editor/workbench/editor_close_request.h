/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_close_request.h
 *
 * PURPOSE:
 *   Represent a governed request to close an editor item.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_CLOSE_REQUEST_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_CLOSE_REQUEST_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorCloseRequest { char resource[UMI_EDITOR_WB_PATH_CAPACITY]; char group_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOpenMode mode; bool force; } UmiEditorWbEditorCloseRequest;
UmiStatus umi_editor_wb_editor_close_request_init(UmiEditorWbEditorCloseRequest *state,const char *resource,const char *group_id);
int umi_editor_wb_editor_close_request_valid(const UmiEditorWbEditorCloseRequest *state);

#ifdef __cplusplus
}
#endif
#endif
