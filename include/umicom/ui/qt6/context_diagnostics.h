/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_diagnostics.h
 *
 * PURPOSE:
 *   Render context-channel routing diagnostics and health in Qt6.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_DIAGNOSTICS_H
#define UMICOM_UI_QT6_CONTEXT_DIAGNOSTICS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 context diagnostics descriptor operation used by this module and its
 * client applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_context_diagnostics_descriptor(void);
/**
 * Initialise qt6 context diagnostics from caller-provided values so later operations
 * receive a known state.
 */
UmiQt6WidgetHandle umi_qt6_context_diagnostics_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
