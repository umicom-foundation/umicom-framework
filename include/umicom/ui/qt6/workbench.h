/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workbench.h
 *
 * PURPOSE:
 *   Render the canonical Umicom workbench aggregate as a Qt6 main-window composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKBENCH_H
#define UMICOM_UI_QT6_WORKBENCH_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 workbench descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_workbench_descriptor(void);
/**
 * Initialise qt6 workbench from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_workbench_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
