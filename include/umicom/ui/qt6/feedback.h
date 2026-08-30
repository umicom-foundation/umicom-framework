/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/feedback.h
 *
 * PURPOSE:
 *   Render progress, result and feedback components through Qt6.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_FEEDBACK_H
#define UMICOM_UI_QT6_FEEDBACK_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_feedback_descriptor(void);
UmiQt6WidgetHandle umi_qt6_feedback_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
