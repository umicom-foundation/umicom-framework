/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/source_control.h
 *
 * PURPOSE:
 *   Render Framework source-control state as a Qt6 change/history surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_SOURCE_CONTROL_H
#define UMICOM_UI_QT6_SOURCE_CONTROL_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 source control descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_source_control_descriptor(void);
/**
 * Initialise qt6 source control from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_source_control_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
