/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/containers_qt6.cpp
 *
 * PURPOSE:
 *   Render canonical container components as Qt6 box/split/tab compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/containers.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.containers",
    "Umicom.Ui.Components.Containers",
    "QSplitter",
    UMI_QT6_NATIVE_SPLITTER,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_containers_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_containers_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
