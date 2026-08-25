/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/media.h
 *
 * PURPOSE:
 *   Render Framework media component contracts through a Qt6 graphics viewport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_MEDIA_H
#define UMICOM_UI_QT6_MEDIA_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_media_descriptor(void);
UmiQt6WidgetHandle umi_qt6_media_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
