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
size_t umi_ui_component_catalogue_count(void);
UmiUiComponentKind umi_ui_component_catalogue_kind(size_t index);
int umi_ui_component_catalogue_contains(UmiUiComponentKind kind);
#endif
