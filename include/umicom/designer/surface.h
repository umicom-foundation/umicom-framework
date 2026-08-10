/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/surface.h
 *
 * PURPOSE:
 *   Interpret semantic x, y, width and height properties for design-surface placement without owning widget geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_SURFACE_H
#define UMICOM_DESIGNER_SURFACE_H
#include "umicom/designer/document.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_designer_surface_get_rect(const UmiDesignerDocument *document,const char *node_id,UmiDesignerRect *out_rect);
UmiStatus umi_designer_surface_set_rect(UmiDesignerDocument *document,const char *node_id,UmiDesignerRect rect);
#ifdef __cplusplus
}
#endif
#endif
