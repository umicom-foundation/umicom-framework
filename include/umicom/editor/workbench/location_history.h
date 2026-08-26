/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/location_history.h
 *
 * PURPOSE:
 *   Maintain a bounded chronological history of editor locations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_LOCATION_HISTORY_H
#define UMICOM_EDITOR_WORKBENCH_LOCATION_HISTORY_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbLocationHistory { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbLocationHistory;
void umi_editor_wb_location_history_init(UmiEditorWbLocationHistory *state);
UmiStatus umi_editor_wb_location_history_append(UmiEditorWbLocationHistory *state,const char *text,uint32_t depth);
UmiStatus umi_editor_wb_location_history_activate(UmiEditorWbLocationHistory *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
