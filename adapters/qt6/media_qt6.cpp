/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/media_qt6.cpp
 *
 * PURPOSE:
 *   Render Framework media component contracts through a Qt6 graphics viewport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/media.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.media",
    "Umicom.Ui.Components.Media",
    "QGraphicsView",
    UMI_QT6_NATIVE_GRAPHICS_VIEW,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_MEDIA | UMI_QT6_CAP_CANVAS
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_media_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_media_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
