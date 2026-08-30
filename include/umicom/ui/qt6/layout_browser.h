/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/layout_browser.h
 *
 * PURPOSE:
 *   Render saved layouts, perspectives and monitor variants as a reusable Qt6 browser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_LAYOUT_BROWSER_H
#define UMICOM_UI_QT6_LAYOUT_BROWSER_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_layout_browser_descriptor(void);
UmiQt6WidgetHandle umi_qt6_layout_browser_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
