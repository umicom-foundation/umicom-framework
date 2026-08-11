/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/catalogue_gtk4.c
 *
 * PURPOSE:
 *   Implement reusable GTK4 rendering for common semantic Umicom components.
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
#include "umicom/ui/gtk4/catalogue.h"
int umi_gtk4_component_is_supported(UmiUiComponentKind k){switch(k){case UMI_UI_COMPONENT_WINDOW:case UMI_UI_COMPONENT_HEADER_BAR:case UMI_UI_COMPONENT_BOX:case UMI_UI_COMPONENT_GRID:case UMI_UI_COMPONENT_BUTTON:case UMI_UI_COMPONENT_LABEL:case UMI_UI_COMPONENT_ENTRY:case UMI_UI_COMPONENT_TEXT_VIEW:case UMI_UI_COMPONENT_LIST:case UMI_UI_COMPONENT_TAB_HOST:case UMI_UI_COMPONENT_PANED:case UMI_UI_COMPONENT_SCROLLED:case UMI_UI_COMPONENT_SEARCH_ENTRY:case UMI_UI_COMPONENT_PROGRESS:case UMI_UI_COMPONENT_SPINNER:case UMI_UI_COMPONENT_CHECK_BUTTON:case UMI_UI_COMPONENT_SWITCH:case UMI_UI_COMPONENT_FRAME:case UMI_UI_COMPONENT_EXPANDER:case UMI_UI_COMPONENT_OVERLAY:case UMI_UI_COMPONENT_STACK:case UMI_UI_COMPONENT_STACK_SWITCHER:case UMI_UI_COMPONENT_PICTURE:case UMI_UI_COMPONENT_VIDEO:case UMI_UI_COMPONENT_DRAWING_SURFACE:return 1;default:return 0;}}
