/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/cross_application_panel_qt6.cpp
 *
 * PURPOSE:
 *   Host registered Studio, Trader, TMS, media and future app surfaces inside Qt6.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/cross_application_panel.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.cross_application_panel",
    "Umicom.Ui.CrossApplicationPanel",
    "QDockWidget",
    UMI_QT6_NATIVE_DOCK_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_cross_application_panel_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_cross_application_panel_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
