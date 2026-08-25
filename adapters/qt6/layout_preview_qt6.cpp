/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/layout_preview_qt6.cpp
 *
 * PURPOSE:
 *   Render toolkit-neutral layout geometry as a Qt6 visual preview scaffold.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/layout_preview.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.layout_preview",
    "Umicom.Ui.LayoutPreview",
    "QGraphicsView",
    UMI_QT6_NATIVE_GRAPHICS_VIEW,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_layout_preview_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_layout_preview_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
