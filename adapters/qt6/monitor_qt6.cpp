/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/monitor_qt6.cpp
 *
 * PURPOSE:
 *   Expose Qt6 screen/monitor presentation state to Framework-owned window restoration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/monitor.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.monitor",
    "Umicom.Ui.MonitorTopology",
    "QListWidget",
    UMI_QT6_NATIVE_LIST,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_MULTI_MONITOR
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_monitor_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_monitor_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
