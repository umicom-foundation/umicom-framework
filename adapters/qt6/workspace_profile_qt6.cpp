/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workspace_profile_qt6.cpp
 *
 * PURPOSE:
 *   Implement the workspace profile qt6 behaviour used by its public contract
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
 * File: adapters/qt6/workspace_profile_qt6.cpp
 *
 * PURPOSE:
 *   Expose workspace-profile selection and saved-layout activation through Qt6 controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workspace_profile.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.workspace_profile",
    "Umicom.Ui.WorkspaceProfile",
    "QListWidget",
    UMI_QT6_NATIVE_LIST,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

/*
 * Provide the qt6 workspace profile descriptor operation used by this module and its
 * client applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_workspace_profile_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

/*
 * Initialise qt6 workspace profile from caller-provided values so later operations receive
 * a known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_workspace_profile_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
