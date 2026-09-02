/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_picker.h
 *
 * PURPOSE:
 *   Render context-link group selection and direction controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_PICKER_H
#define UMICOM_UI_QT6_CONTEXT_PICKER_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 context picker descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_context_picker_descriptor(void);
/**
 * Initialise qt6 context picker from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_context_picker_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
