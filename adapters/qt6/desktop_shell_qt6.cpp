/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/desktop_shell_qt6.cpp
 *
 * PURPOSE:
 *   Render Umicom Desk/Desktop shell composition using the same Framework workbench contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/desktop_shell.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.desktop_shell",
    "Umicom.Desktop",
    "QMainWindow",
    UMI_QT6_NATIVE_MAIN_WINDOW,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT | UMI_QT6_CAP_MULTI_MONITOR
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_desktop_shell_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_desktop_shell_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
