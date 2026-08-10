/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/palette.h
 *
 * PURPOSE:
 *   Expose a filtered component palette derived from the declarative component registry and plug-in contributions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_PALETTE_H
#define UMICOM_DESIGNER_PALETTE_H
#include "umicom/designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDesignerPalette { UmiDeclComponentDescriptor *items; size_t count; } UmiDesignerPalette;
UmiStatus umi_designer_palette_build(const UmiDeclComponentRegistry *registry,const char *category_filter,UmiDesignerPalette *out_palette);
void umi_designer_palette_dispose(UmiDesignerPalette *palette);
#ifdef __cplusplus
}
#endif
#endif
