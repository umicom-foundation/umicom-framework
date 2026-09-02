/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/declarative_renderer_qt6.cpp
 *
 * PURPOSE:
 *   Implement the declarative renderer qt6 behaviour used by its public contract
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
 * File: adapters/qt6/declarative_renderer_qt6.cpp
 *
 * PURPOSE:
 *   Render Framework declarative UI nodes through Qt6 widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/declarative_renderer.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.declarative_renderer",
    "Umicom.Ui.Declarative",
    "QWidget",
    UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

/*
 * Provide the qt6 declarative renderer descriptor operation used by this module and its
 * client applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_declarative_renderer_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

/*
 * Initialise qt6 declarative renderer from caller-provided values so later operations
 * receive a known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_declarative_renderer_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
