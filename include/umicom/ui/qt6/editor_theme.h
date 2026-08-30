/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/editor_theme.h
 *
 * PURPOSE:
 *   Bridge semantic editor appearance tokens into Qt6 editor presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_EDITOR_THEME_H
#define UMICOM_UI_QT6_EDITOR_THEME_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_editor_theme_descriptor(void);
UmiQt6WidgetHandle umi_qt6_editor_theme_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
