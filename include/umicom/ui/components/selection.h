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
typedef struct UmiUiComponentSelection { size_t index; bool has_selection; } UmiUiComponentSelection;
UmiUiComponentSelection umi_ui_component_selection_none(void);
#endif
