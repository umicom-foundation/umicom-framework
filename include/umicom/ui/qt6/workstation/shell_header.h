/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/shell_header.h
 *
 * PURPOSE:
 *   Render the universal workstation shell header for Studio, Trader, media, design and system applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_SHELL_HEADER_H
#define UMICOM_UI_QT6_WORKSTATION_SHELL_HEADER_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws shell header descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_shell_header_descriptor(void);
/**
 * Initialise qt6 ws shell header from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_shell_header_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
