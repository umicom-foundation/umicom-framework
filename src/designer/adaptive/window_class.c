/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/window_class.c
 *
 * PURPOSE:
 *   Describe designer preview window classes and minimum logical dimensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/window_class.h"
#include <string.h>

/* Store one window class after validating positive minimum bounds. */
UmiStatus umi_designer_window_class_init(UmiDesignerAdaptiveWindowClass *window_class,
                                         const char *class_id,
                                         int32_t minimum_width,
                                         int32_t minimum_height,
                                         int detachable)
{
    if(window_class==NULL||class_id==NULL||minimum_width<=0||minimum_height<=0)return UMI_STATUS_INVALID_ARGUMENT;
    memset(window_class,0,sizeof *window_class);
    if(umi_designer_adaptive_copy_text(window_class->class_id,sizeof window_class->class_id,class_id)!=UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    window_class->minimum_width=minimum_width;
    window_class->minimum_height=minimum_height;
    window_class->detachable=detachable!=0;
    return UMI_STATUS_OK;
}
/* Check preview/runtime logical bounds against the authored window class. */
int umi_designer_window_class_matches(const UmiDesignerAdaptiveWindowClass *window_class,
                                      UmiAdaptiveViewport viewport)
{
    return window_class!=NULL&&viewport.width>=window_class->minimum_width&&viewport.height>=window_class->minimum_height;
}
