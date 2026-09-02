/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/dialog.h
 *
 * PURPOSE:
 *   Render Framework dialog requests using a Qt6 dialog host boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_DIALOG_H
#define UMICOM_UI_QT6_DIALOG_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 dialog descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_dialog_descriptor(void);
/**
 * Initialise qt6 dialog from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_dialog_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
