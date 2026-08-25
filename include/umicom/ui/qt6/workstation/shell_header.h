/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/shell_header.h
 *
 * PURPOSE:
 *   Render the universal workstation shell header for Studio, Trader, media, design and system applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_SHELL_HEADER_H
#define UMICOM_UI_QT6_WORKSTATION_SHELL_HEADER_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_shell_header_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_shell_header_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
