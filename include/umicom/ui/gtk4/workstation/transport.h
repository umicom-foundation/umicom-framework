/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/transport.h
 *
 * PURPOSE:
 *   Render media/replay transport controls from the generic transport model without embedding domain logic in GTK4.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_TRANSPORT_H
#define UMICOM_UI_GTK4_WORKSTATION_TRANSPORT_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/transport_model.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 ws transport from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_transport_create(const UmiWsTransportModel *model);


#ifdef __cplusplus
}
#endif
#endif
