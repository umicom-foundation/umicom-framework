/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/shell.h
 *
 * PURPOSE:
 *   Render the Framework application shell through Qt6 without moving reusable shell state into products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_SHELL_H
#define UMICOM_UI_QT6_SHELL_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 shell descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_shell_descriptor(void);
/**
 * Initialise qt6 shell from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_shell_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
