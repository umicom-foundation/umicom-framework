/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/layout.h
 *
 * PURPOSE:
 *   Define reusable layout constraints independent from GTK containers.
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

#ifndef UMICOM_UI_COMPONENTS_LAYOUT_H
#define UMICOM_UI_COMPONENTS_LAYOUT_H
#include <stdbool.h>
#include <stdint.h>
typedef struct UmiUiComponentLayout { int32_t margin_start,margin_end,margin_top,margin_bottom; int32_t row,column,row_span,column_span; bool homogeneous; } UmiUiComponentLayout;
UmiUiComponentLayout umi_ui_component_layout_default(void);
#endif
