/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workspace_profile.h
 *
 * PURPOSE:
 *   Expose workspace-profile selection and saved-layout activation through Qt6 controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSPACE_PROFILE_H
#define UMICOM_UI_QT6_WORKSPACE_PROFILE_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 workspace profile descriptor operation used by this module and its
 * client applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_workspace_profile_descriptor(void);
/**
 * Initialise qt6 workspace profile from caller-provided values so later operations receive
 * a known state.
 */
UmiQt6WidgetHandle umi_qt6_workspace_profile_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
