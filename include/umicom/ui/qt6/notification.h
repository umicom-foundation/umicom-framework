/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/notification.h
 *
 * PURPOSE:
 *   Render Framework notifications and transient operational feedback using Qt6.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_NOTIFICATION_H
#define UMICOM_UI_QT6_NOTIFICATION_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_notification_descriptor(void);
UmiQt6WidgetHandle umi_qt6_notification_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
