/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/keybinding.h
 *
 * PURPOSE:
 *   Expose Framework keybinding state and shortcut discoverability through Qt6.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_KEYBINDING_H
#define UMICOM_UI_QT6_KEYBINDING_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_keybinding_descriptor(void);
UmiQt6WidgetHandle umi_qt6_keybinding_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
