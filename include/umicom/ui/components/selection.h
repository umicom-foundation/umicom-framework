/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/selection.h
 *
 * PURPOSE:
 *   Track a selected row/tab/item without exposing GtkSelectionModel to product logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_SELECTION_H
#define UMICOM_UI_COMPONENTS_SELECTION_H
#include <stddef.h>
#include <stdbool.h>
/**
 * Represent the ui component selection data shared with callers of this public contract.
 */
typedef struct UmiUiComponentSelection { size_t index; bool has_selection; } UmiUiComponentSelection;
/**
 * Provide the ui component selection none operation used by this module and its client
 * applications.
 */
UmiUiComponentSelection umi_ui_component_selection_none(void);
#endif
