/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_badge.h
 *
 * PURPOSE:
 *   Render typed context-link group identity with colour-independent labels/icons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_BADGE_H
#define UMICOM_UI_QT6_CONTEXT_BADGE_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_context_badge_descriptor(void);
UmiQt6WidgetHandle umi_qt6_context_badge_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
