/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/media.h
 *
 * PURPOSE:
 *   Render Framework media component contracts through a Qt6 graphics viewport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_MEDIA_H
#define UMICOM_UI_QT6_MEDIA_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 media descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_media_descriptor(void);
/**
 * Initialise qt6 media from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_media_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
