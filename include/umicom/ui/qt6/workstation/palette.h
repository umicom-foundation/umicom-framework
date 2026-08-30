/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/palette.h
 *
 * PURPOSE:
 *   Render searchable component/tool palettes for designers and editors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_PALETTE_H
#define UMICOM_UI_QT6_WORKSTATION_PALETTE_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/palette_model.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_palette_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_palette_create(const UmiWsPaletteModel *model);

#ifdef __cplusplus
}
#endif
#endif
