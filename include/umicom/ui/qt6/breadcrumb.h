/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/breadcrumb.h
 *
 * PURPOSE:
 *   Render semantic breadcrumb navigation for project, symbol, media and application contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_BREADCRUMB_H
#define UMICOM_UI_QT6_BREADCRUMB_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 breadcrumb descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_breadcrumb_descriptor(void);
/**
 * Initialise qt6 breadcrumb from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_breadcrumb_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
