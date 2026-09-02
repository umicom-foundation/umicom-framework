/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/clipboard.h
 *
 * PURPOSE:
 *   Provide a Qt6 clipboard-facing presentation/service boundary without exposing Qt to the C core.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CLIPBOARD_H
#define UMICOM_UI_QT6_CLIPBOARD_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 clipboard descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_clipboard_descriptor(void);
/**
 * Initialise qt6 clipboard from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_clipboard_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
