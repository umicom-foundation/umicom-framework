/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/cross_application_panel.h
 *
 * PURPOSE:
 *   Host registered Studio, Trader, TMS, media and future app surfaces inside Qt6.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CROSS_APPLICATION_PANEL_H
#define UMICOM_UI_QT6_CROSS_APPLICATION_PANEL_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 cross application panel descriptor operation used by this module and its
 * client applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_cross_application_panel_descriptor(void);
/**
 * Initialise qt6 cross application panel from caller-provided values so later operations
 * receive a known state.
 */
UmiQt6WidgetHandle umi_qt6_cross_application_panel_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
