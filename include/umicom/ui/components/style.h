/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/style.h
 *
 * PURPOSE:
 *   Describe component styling hints without embedding CSS or toolkit objects in product-neutral code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_STYLE_H
#define UMICOM_UI_COMPONENTS_STYLE_H
#include <stdbool.h>
#include "umicom/ui/components/component.h"
typedef struct UmiUiComponentStyle { char primary_class[UMI_UI_COMPONENT_CLASS_CAPACITY]; bool suggested_action; bool destructive_action; bool flat; } UmiUiComponentStyle;
UmiUiComponentStyle umi_ui_component_style_default(void);
#endif
