/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/component_factory_qt6.cpp
 *
 * PURPOSE:
 *   Implement the component factory qt6 behaviour used by its public contract
 *   and client applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/component_factory_qt6.cpp
 *
 * PURPOSE:
 *   Expose Framework component-factory creation through the Qt6 adapter boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/component_factory.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.component_factory",
    "Umicom.Ui.Components.Factory",
    "QWidget",
    UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

/*
 * Provide the qt6 component factory descriptor operation used by this module and its
 * client applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_component_factory_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

/*
 * Initialise qt6 component factory from caller-provided values so later operations receive
 * a known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_component_factory_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
