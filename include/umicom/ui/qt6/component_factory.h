/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/component_factory.h
 *
 * PURPOSE:
 *   Expose Framework component-factory creation through the Qt6 adapter boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_COMPONENT_FACTORY_H
#define UMICOM_UI_QT6_COMPONENT_FACTORY_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 component factory descriptor operation used by this module and its
 * client applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_component_factory_descriptor(void);
/**
 * Initialise qt6 component factory from caller-provided values so later operations receive
 * a known state.
 */
UmiQt6WidgetHandle umi_qt6_component_factory_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
