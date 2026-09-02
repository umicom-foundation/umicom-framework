/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/tab_navigation.h
 *
 * PURPOSE:
 *   Calculate next/previous tab positions with optional wrap-around.
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
#ifndef UMICOM_EDITOR_WORKBENCH_TAB_NAVIGATION_H
#define UMICOM_EDITOR_WORKBENCH_TAB_NAVIGATION_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the editor wb tab navigation next operation used by this module and its client
 * applications.
 */
size_t umi_editor_wb_tab_navigation_next(size_t current,size_t count,int direction,bool wrap);

#ifdef __cplusplus
}
#endif
#endif
