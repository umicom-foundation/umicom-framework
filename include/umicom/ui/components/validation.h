/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/validation.h
 *
 * PURPOSE:
 *   Validate semantic component combinations before a frontend adapter creates widgets.
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

#ifndef UMICOM_UI_COMPONENTS_VALIDATION_H
#define UMICOM_UI_COMPONENTS_VALIDATION_H
#include "umicom/ui/components/component.h"
UmiStatus umi_ui_component_validate_parent_child(UmiUiComponentKind parent,UmiUiComponentKind child);
#endif
