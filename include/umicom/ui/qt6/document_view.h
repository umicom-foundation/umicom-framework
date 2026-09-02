/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/document_view.h
 *
 * PURPOSE:
 *   Render Framework document-view state as a Qt6 document surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_DOCUMENT_VIEW_H
#define UMICOM_UI_QT6_DOCUMENT_VIEW_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 document view descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_document_view_descriptor(void);
/**
 * Initialise qt6 document view from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_document_view_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
