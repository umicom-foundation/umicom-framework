/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/status_strip.h
 *
 * PURPOSE:
 *   Render workstation status items with severity and left/right alignment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_STATUS_STRIP_H
#define UMICOM_UI_QT6_WORKSTATION_STATUS_STRIP_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/status_strip.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws status strip descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_status_strip_descriptor(void);
/**
 * Initialise qt6 ws status strip from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_status_strip_create(const UmiWsStatusStrip *strip);

#ifdef __cplusplus
}
#endif
#endif
