/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/native_factory.h
 *
 * PURPOSE:
 *   Instantiate catalogued GTK4 widget types through GtkBuilder while applying
 *   the same common semantic component state as direct Framework factories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_NATIVE_FACTORY_H
#define UMICOM_UI_GTK4_NATIVE_FACTORY_H

#include <gtk/gtk.h>

#include "umicom/ui/components/component.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 native widget from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_native_widget_create(
    const char *gtk_type_name,
    const UmiUiComponentSpec *common_spec,
    GError **error);


#ifdef __cplusplus
}
#endif
#endif
