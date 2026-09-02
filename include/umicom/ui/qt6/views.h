/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/views.h
 *
 * PURPOSE:
 *   Render general Framework view components in a Qt6 reusable host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_VIEWS_H
#define UMICOM_UI_QT6_VIEWS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 views descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_views_descriptor(void);
/**
 * Initialise qt6 views from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_views_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
