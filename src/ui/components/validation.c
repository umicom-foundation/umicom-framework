/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/validation.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/validation.h"

UmiStatus umi_ui_component_validate_parent_child(UmiUiComponentKind parent,
                                                 UmiUiComponentKind child)
{
    if (parent == UMI_UI_COMPONENT_BUTTON ||
        parent == UMI_UI_COMPONENT_LABEL ||
        parent == UMI_UI_COMPONENT_ENTRY ||
        parent == UMI_UI_COMPONENT_PROGRESS ||
        parent == UMI_UI_COMPONENT_SPINNER) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (child == UMI_UI_COMPONENT_WINDOW && parent != UMI_UI_COMPONENT_CUSTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
