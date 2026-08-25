/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/desktop_shell.h
 *
 * PURPOSE:
 *   Render Umicom Desk/Desktop shell composition using the same Framework workbench contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_DESKTOP_SHELL_H
#define UMICOM_UI_QT6_DESKTOP_SHELL_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_desktop_shell_descriptor(void);
UmiQt6WidgetHandle umi_qt6_desktop_shell_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
