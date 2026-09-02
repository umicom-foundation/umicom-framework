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
/**
 * Represent the editor wb code map data shared with callers of this public contract.
 */
typedef struct UmiEditorWbCodeMap { UmiEditorWbCodeMapSegment segments[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; } UmiEditorWbCodeMap;
/**
 * Initialise editor wb code map from caller-provided values so later operations receive a
 * known state.
 */
void umi_editor_wb_code_map_init(UmiEditorWbCodeMap *map);
/**
 * Add editor wb code map only after its inputs and available capacity have been checked.
 */
UmiStatus umi_editor_wb_code_map_add(UmiEditorWbCodeMap *map,const UmiEditorWbCodeMapSegment *segment);
/**
 * Provide the editor wb code map find line operation used by this module and its client
 * applications.
 */
const UmiEditorWbCodeMapSegment *umi_editor_wb_code_map_find_line(const UmiEditorWbCodeMap *map,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
