/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/editor.h
 *
 * PURPOSE:
 *   Render Framework editor/document models using a Qt6 text-edit surface adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_EDITOR_H
#define UMICOM_UI_QT6_EDITOR_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 editor descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_editor_descriptor(void);
/**
 * Initialise qt6 editor from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_editor_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
