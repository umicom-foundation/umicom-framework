/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/transport.h
 *
 * PURPOSE:
 *   Render media/replay transport state and playback controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_TRANSPORT_H
#define UMICOM_UI_QT6_WORKSTATION_TRANSPORT_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/transport_model.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_transport_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_transport_create(const UmiWsTransportModel *transport);

#ifdef __cplusplus
}
#endif
#endif
