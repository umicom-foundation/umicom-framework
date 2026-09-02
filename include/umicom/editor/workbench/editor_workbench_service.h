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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_WORKBENCH_SERVICE_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_WORKBENCH_SERVICE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor workbench service data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorWorkbenchService { bool editor_ready; bool ui_ready; bool navigation_ready; uint64_t opened; uint64_t closed; uint64_t restored; } UmiEditorWbEditorWorkbenchService;
/**
 * Initialise editor wb editor workbench service from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_editor_workbench_service_init(UmiEditorWbEditorWorkbenchService *service);
/**
 * Provide the editor wb editor workbench service ready operation used by this module and
 * its client applications.
 */
int umi_editor_wb_editor_workbench_service_ready(const UmiEditorWbEditorWorkbenchService *service);
/**
 * Provide the editor wb editor workbench service record open operation used by this module
 * and its client applications.
 */
void umi_editor_wb_editor_workbench_service_record_open(UmiEditorWbEditorWorkbenchService *service);
/**
 * Provide the editor wb editor workbench service record close operation used by this
 * module and its client applications.
 */
void umi_editor_wb_editor_workbench_service_record_close(UmiEditorWbEditorWorkbenchService *service);
/**
 * Provide the editor wb editor workbench service record restore operation used by this
 * module and its client applications.
 */
void umi_editor_wb_editor_workbench_service_record_restore(UmiEditorWbEditorWorkbenchService *service);

#ifdef __cplusplus
}
#endif
#endif
