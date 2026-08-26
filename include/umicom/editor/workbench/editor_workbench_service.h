/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_workbench_service.h
 *
 * PURPOSE:
 *   Aggregate editor-workbench readiness and lifecycle counters for thin IDE frontends.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_WORKBENCH_SERVICE_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_WORKBENCH_SERVICE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorWorkbenchService { bool editor_ready; bool ui_ready; bool navigation_ready; uint64_t opened; uint64_t closed; uint64_t restored; } UmiEditorWbEditorWorkbenchService;
void umi_editor_wb_editor_workbench_service_init(UmiEditorWbEditorWorkbenchService *service);
int umi_editor_wb_editor_workbench_service_ready(const UmiEditorWbEditorWorkbenchService *service);
void umi_editor_wb_editor_workbench_service_record_open(UmiEditorWbEditorWorkbenchService *service);
void umi_editor_wb_editor_workbench_service_record_close(UmiEditorWbEditorWorkbenchService *service);
void umi_editor_wb_editor_workbench_service_record_restore(UmiEditorWbEditorWorkbenchService *service);

#ifdef __cplusplus
}
#endif
#endif
