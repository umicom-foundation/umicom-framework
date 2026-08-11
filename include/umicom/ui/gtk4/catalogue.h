/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/catalogue.h
 *
 * PURPOSE:
 *   Report which semantic component kinds the GTK4 factory supports.
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

#ifndef UMICOM_UI_GTK4_CATALOGUE_H
#define UMICOM_UI_GTK4_CATALOGUE_H
#include "umicom/ui/components/component.h"
int umi_gtk4_component_is_supported(UmiUiComponentKind kind);
#endif
