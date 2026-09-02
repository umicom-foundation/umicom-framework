/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/code_map_segment.h
 *
 * PURPOSE:
 *   Describe one semantic segment of a code-map overview.
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
#ifndef UMICOM_EDITOR_WORKBENCH_CODE_MAP_SEGMENT_H
#define UMICOM_EDITOR_WORKBENCH_CODE_MAP_SEGMENT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb code map segment data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbCodeMapSegment { char segment_id[UMI_EDITOR_WB_ID_CAPACITY]; uint32_t first_line; uint32_t last_line; UmiEditorWbSeverity severity; } UmiEditorWbCodeMapSegment;
/**
 * Initialise editor wb code map segment from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_code_map_segment_init(UmiEditorWbCodeMapSegment *segment,const char *id,uint32_t first_line,uint32_t last_line,UmiEditorWbSeverity severity);
/**
 * Provide the editor wb code map segment contains operation used by this module and its
 * client applications.
 */
int umi_editor_wb_code_map_segment_contains(const UmiEditorWbCodeMapSegment *segment,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
