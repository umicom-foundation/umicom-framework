/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/catalogue.h
 *
 * PURPOSE:
 *   Report which semantic component kinds the GTK4 factory supports.
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

#ifndef UMICOM_UI_GTK4_CATALOGUE_H
#define UMICOM_UI_GTK4_CATALOGUE_H
#include "umicom/ui/components/component.h"
/**
 * Provide the gtk4 component is supported operation used by this module and its client
 * applications.
 */
int umi_gtk4_component_is_supported(UmiUiComponentKind kind);
#endif
