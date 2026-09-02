/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/accessibility.h
 *
 * PURPOSE:
 *   Apply Framework accessibility semantics to Qt6 presentation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_ACCESSIBILITY_H
#define UMICOM_UI_QT6_ACCESSIBILITY_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 accessibility descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_accessibility_descriptor(void);
/**
 * Initialise qt6 accessibility from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_accessibility_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
