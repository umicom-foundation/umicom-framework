/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_toolbar.h
 *
 * PURPOSE:
 *   Render context-link controls in a reusable Qt6 toolbar.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_TOOLBAR_H
#define UMICOM_UI_QT6_CONTEXT_TOOLBAR_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_context_toolbar_descriptor(void);
UmiQt6WidgetHandle umi_qt6_context_toolbar_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
