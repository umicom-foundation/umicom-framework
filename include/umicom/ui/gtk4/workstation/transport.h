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

/**
 * Initialise gtk4 ws transport from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_transport_create(const UmiWsTransportModel *model);

#endif
