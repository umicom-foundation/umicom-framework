/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/indentation_guides.h
 *
 * PURPOSE:
 *   Calculate semantic indentation-guide depth for whitespace columns.
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
#ifndef UMICOM_EDITOR_WORKBENCH_INDENTATION_GUIDES_H
#define UMICOM_EDITOR_WORKBENCH_INDENTATION_GUIDES_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

uint32_t umi_editor_wb_indentation_guides_depth(uint32_t leading_columns,uint32_t tab_width);

#ifdef __cplusplus
}
#endif
#endif
