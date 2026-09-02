/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/media_adaptation.h
 *
 * PURPOSE:
 *   Resolve media-preview and transport placement for adaptive creative application layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_MEDIA_ADAPTATION_H
#define UMICOM_DESIGNER_ADAPTIVE_MEDIA_ADAPTATION_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer media adaptation data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerMediaAdaptation {
    int transport_overlay;
    int timeline_below;
    int inspector_overlay;
    double preview_fraction;
} UmiDesignerMediaAdaptation;
/* Resolve creative-media surface composition for the active size class. */
UmiDesignerMediaAdaptation umi_designer_media_adaptation_resolve(UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
