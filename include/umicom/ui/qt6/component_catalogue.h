/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/component_catalogue.h
 *
 * PURPOSE:
 *   Render/search the Framework component catalogue through Qt6.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_COMPONENT_CATALOGUE_H
#define UMICOM_UI_QT6_COMPONENT_CATALOGUE_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 component catalogue descriptor operation used by this module and its
 * client applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_component_catalogue_descriptor(void);
/**
 * Initialise qt6 component catalogue from caller-provided values so later operations
 * receive a known state.
 */
UmiQt6WidgetHandle umi_qt6_component_catalogue_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
