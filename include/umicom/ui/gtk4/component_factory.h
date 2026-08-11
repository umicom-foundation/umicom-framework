/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/component_factory.h
 *
 * PURPOSE:
 *   Create GTK4 widgets from toolkit-neutral Umicom component specifications.
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

#ifndef UMICOM_UI_GTK4_COMPONENT_FACTORY_H
#define UMICOM_UI_GTK4_COMPONENT_FACTORY_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#include "umicom/ui/components/component.h"
GtkWidget *umi_gtk4_component_create(const UmiUiComponentSpec *spec);
UmiStatus umi_gtk4_component_apply_common(GtkWidget *widget,const UmiUiComponentSpec *spec);
#endif
