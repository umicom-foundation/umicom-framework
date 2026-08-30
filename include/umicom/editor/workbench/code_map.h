/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/code_map.h
 *
 * PURPOSE:
 *   Maintain semantic line-range segments used by a code-map overview.
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
#ifndef UMICOM_EDITOR_WORKBENCH_CODE_MAP_H
#define UMICOM_EDITOR_WORKBENCH_CODE_MAP_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/editor/workbench/code_map_segment.h"
typedef struct UmiEditorWbCodeMap { UmiEditorWbCodeMapSegment segments[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; } UmiEditorWbCodeMap;
void umi_editor_wb_code_map_init(UmiEditorWbCodeMap *map);
UmiStatus umi_editor_wb_code_map_add(UmiEditorWbCodeMap *map,const UmiEditorWbCodeMapSegment *segment);
const UmiEditorWbCodeMapSegment *umi_editor_wb_code_map_find_line(const UmiEditorWbCodeMap *map,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
