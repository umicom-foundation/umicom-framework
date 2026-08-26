/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/session_snapshot.h
 *
 * PURPOSE:
 *   Capture editor-session counts, active identity and revision metadata.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_SESSION_SNAPSHOT_H
#define UMICOM_EDITOR_WORKBENCH_SESSION_SNAPSHOT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbSessionSnapshot { char active_id[UMI_EDITOR_WB_ID_CAPACITY]; size_t item_count; size_t group_count; uint64_t revision; uint64_t fingerprint; } UmiEditorWbSessionSnapshot;
UmiStatus umi_editor_wb_session_snapshot_capture(UmiEditorWbSessionSnapshot *state,const char *active_id,size_t item_count,size_t group_count,uint64_t revision); int umi_editor_wb_session_snapshot_valid(const UmiEditorWbSessionSnapshot *state);

#ifdef __cplusplus
}
#endif
#endif
