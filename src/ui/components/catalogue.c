/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/catalogue.c
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
#include "umicom/ui/components/catalogue.h"

static const UmiUiComponentKind KINDS[] = {
    UMI_UI_COMPONENT_WINDOW,
    UMI_UI_COMPONENT_HEADER_BAR,
    UMI_UI_COMPONENT_BOX,
    UMI_UI_COMPONENT_GRID,
    UMI_UI_COMPONENT_BUTTON,
    UMI_UI_COMPONENT_LABEL,
    UMI_UI_COMPONENT_ENTRY,
    UMI_UI_COMPONENT_TEXT_VIEW,
    UMI_UI_COMPONENT_SOURCE_VIEW,
    UMI_UI_COMPONENT_LIST,
    UMI_UI_COMPONENT_COLUMN_VIEW,
    UMI_UI_COMPONENT_TAB_HOST,
    UMI_UI_COMPONENT_PANED,
    UMI_UI_COMPONENT_SCROLLED,
    UMI_UI_COMPONENT_POPOVER,
    UMI_UI_COMPONENT_SEARCH_ENTRY,
    UMI_UI_COMPONENT_PROGRESS,
    UMI_UI_COMPONENT_SPINNER,
    UMI_UI_COMPONENT_CHECK_BUTTON,
    UMI_UI_COMPONENT_SWITCH,
    UMI_UI_COMPONENT_DROP_DOWN,
    UMI_UI_COMPONENT_SEPARATOR,
    UMI_UI_COMPONENT_FRAME,
    UMI_UI_COMPONENT_EXPANDER,
    UMI_UI_COMPONENT_OVERLAY,
    UMI_UI_COMPONENT_STACK,
    UMI_UI_COMPONENT_STACK_SWITCHER,
    UMI_UI_COMPONENT_PICTURE,
    UMI_UI_COMPONENT_VIDEO,
    UMI_UI_COMPONENT_DRAWING_SURFACE
};

size_t umi_ui_component_catalogue_count(void)
{
    return sizeof(KINDS) / sizeof(KINDS[0]);
}

UmiUiComponentKind umi_ui_component_catalogue_kind(size_t index)
{
    return index < umi_ui_component_catalogue_count() ? KINDS[index] : 0;
}

int umi_ui_component_catalogue_contains(UmiUiComponentKind kind)
{
    for (size_t index = 0U; index < umi_ui_component_catalogue_count(); ++index) {
        if (KINDS[index] == kind) {
            return 1;
        }
    }
    return 0;
}
