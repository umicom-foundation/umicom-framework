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

/**
 * Provide the qt6 editor theme descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_editor_theme_descriptor(void);
/**
 * Initialise qt6 editor theme from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_editor_theme_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
