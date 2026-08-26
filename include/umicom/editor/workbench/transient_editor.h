/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/transient_editor.h
 *
 * PURPOSE:
 *   Track ephemeral editor activity and whether it should remain retained.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_TRANSIENT_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_TRANSIENT_EDITOR_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbTransientEditor { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbTransientEditor;
UmiStatus umi_editor_wb_transient_editor_init(UmiEditorWbTransientEditor *state,const char *id,const char *text); UmiStatus umi_editor_wb_transient_editor_set_values(UmiEditorWbTransientEditor *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_transient_editor_valid(const UmiEditorWbTransientEditor *state);

#ifdef __cplusplus
}
#endif
#endif
