/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/action_dispatch_qt6.cpp
 *
 * PURPOSE:
 *   Bind Framework command/action dispatch to Qt6 presentation actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/action_dispatch.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.action_dispatch",
    "Umicom.Ui.Action",
    "QListWidget",
    UMI_QT6_NATIVE_LIST,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_action_dispatch_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_action_dispatch_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
