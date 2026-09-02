/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/catalogue.h
 *
 * PURPOSE:
 *   Describe which semantic component kinds are part of the reusable Framework catalogue.
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

#ifndef UMICOM_UI_COMPONENTS_CATALOGUE_H
#define UMICOM_UI_COMPONENTS_CATALOGUE_H
#include <stddef.h>
#include "umicom/ui/components/component.h"
/**
 * Return the number of records represented by ui component catalogue without changing
 * their state.
 */
size_t umi_ui_component_catalogue_count(void);
/**
 * Provide the ui component catalogue kind operation used by this module and its client
 * applications.
 */
UmiUiComponentKind umi_ui_component_catalogue_kind(size_t index);
/**
 * Provide the ui component catalogue contains operation used by this module and its client
 * applications.
 */
int umi_ui_component_catalogue_contains(UmiUiComponentKind kind);
#endif
