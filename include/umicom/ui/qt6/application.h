/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/application.h
 *
 * PURPOSE:
 *   Bind Framework application presentation state to a Qt6 application-facing host surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_APPLICATION_H
#define UMICOM_UI_QT6_APPLICATION_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 application descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_application_descriptor(void);
/**
 * Initialise qt6 application from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_application_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
