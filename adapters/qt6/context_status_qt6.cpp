/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/context_status_qt6.cpp
 *
 * PURPOSE:
 *   Render linked-context state in a compact Qt6 status surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/context_status.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.context_status",
    "Umicom.ContextChannel.Status",
    "QStatusBar",
    UMI_QT6_NATIVE_STATUS_BAR,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_CONTEXT
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_context_status_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_context_status_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
