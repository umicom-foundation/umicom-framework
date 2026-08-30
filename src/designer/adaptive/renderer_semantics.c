/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/renderer_semantics.c
 *
 * PURPOSE:
 *   Describe semantic capabilities expected from GTK4, Qt6 and Native Web adaptive renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/renderer_semantics.h"
#include <string.h>

/* Encode only semantic capability expectations; renderer implementation details remain adapter-owned. */
UmiStatus umi_designer_renderer_semantics_default(UmiRadTargetKind renderer,
                                                  UmiDesignerRendererSemantics *out_semantics)
{
    if(out_semantics==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_semantics,0,sizeof *out_semantics);
    out_semantics->renderer=renderer;
    switch(renderer){
    case UMI_RAD_TARGET_GTK4:
    case UMI_RAD_TARGET_QT6:
        out_semantics->supports_hover=1; out_semantics->supports_touch=1;
        out_semantics->supports_detach=1; out_semantics->supports_multiple_windows=1; break;
    case UMI_RAD_TARGET_NATIVE_WEB:
        out_semantics->supports_hover=1; out_semantics->supports_touch=1;
        out_semantics->supports_detach=0; out_semantics->supports_multiple_windows=0; break;
    case UMI_RAD_TARGET_ABSTRACT:
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
