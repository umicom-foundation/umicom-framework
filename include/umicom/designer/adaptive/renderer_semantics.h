/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/renderer_semantics.h
 *
 * PURPOSE:
 *   Describe semantic capabilities expected from GTK4, Qt6 and Native Web adaptive renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_RENDERER_SEMANTICS_H
#define UMICOM_DESIGNER_ADAPTIVE_RENDERER_SEMANTICS_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerRendererSemantics {
    UmiRadTargetKind renderer;
    int supports_hover;
    int supports_touch;
    int supports_detach;
    int supports_multiple_windows;
} UmiDesignerRendererSemantics;
/* Return the baseline semantic capability profile for a Framework renderer target. */
UmiStatus umi_designer_renderer_semantics_default(UmiRadTargetKind renderer,
                                                  UmiDesignerRendererSemantics *out_semantics);

#ifdef __cplusplus
}
#endif
#endif
