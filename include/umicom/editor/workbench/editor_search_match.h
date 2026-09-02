/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_search_match.h
 *
 * PURPOSE:
 *   Describe one in-editor search match and its selected state.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_SEARCH_MATCH_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_SEARCH_MATCH_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor search match data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorSearchMatch { UmiEditorWbRange range; bool selected; } UmiEditorWbEditorSearchMatch;
/**
 * Initialise editor wb editor search match from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_search_match_init(UmiEditorWbEditorSearchMatch *match,UmiEditorWbRange range,bool selected);

#ifdef __cplusplus
}
#endif
#endif
