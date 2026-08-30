/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_location.h
 *
 * PURPOSE:
 *   Represent a validated file, line and column editor location.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_LOCATION_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_LOCATION_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorLocation { char path[UMI_EDITOR_WB_PATH_CAPACITY]; UmiEditorWbPosition position; } UmiEditorWbEditorLocation;
UmiStatus umi_editor_wb_editor_location_init(UmiEditorWbEditorLocation *location,const char *path,uint32_t line,uint32_t column);
int umi_editor_wb_editor_location_valid(const UmiEditorWbEditorLocation *location);

#ifdef __cplusplus
}
#endif
#endif
