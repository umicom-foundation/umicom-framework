/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/validation.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
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
#include "umicom/ui/components/validation.h"

/*
 * Provide the ui component validate parent child operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_component_validate_parent_child(UmiUiComponentKind parent,
                                                 UmiUiComponentKind child)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent == UMI_UI_COMPONENT_BUTTON ||
        parent == UMI_UI_COMPONENT_LABEL ||
        parent == UMI_UI_COMPONENT_ENTRY ||
        parent == UMI_UI_COMPONENT_PROGRESS ||
        parent == UMI_UI_COMPONENT_SPINNER) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (child == UMI_UI_COMPONENT_WINDOW && parent != UMI_UI_COMPONENT_CUSTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
