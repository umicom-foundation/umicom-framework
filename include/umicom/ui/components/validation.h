/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/validation.h
 *
 * PURPOSE:
 *   Validate semantic component combinations before a frontend adapter creates widgets.
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

#ifndef UMICOM_UI_COMPONENTS_VALIDATION_H
#define UMICOM_UI_COMPONENTS_VALIDATION_H
#include "umicom/ui/components/component.h"
/**
 * Provide the ui component validate parent child operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_component_validate_parent_child(UmiUiComponentKind parent,UmiUiComponentKind child);
#endif
