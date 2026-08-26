/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/window_class.h
 *
 * PURPOSE:
 *   Describe designer preview window classes and minimum logical dimensions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_WINDOW_CLASS_H
#define UMICOM_DESIGNER_ADAPTIVE_WINDOW_CLASS_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAdaptiveWindowClass {
    char class_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    int32_t minimum_width;
    int32_t minimum_height;
    int detachable;
} UmiDesignerAdaptiveWindowClass;
/* Initialise one reusable adaptive window-class descriptor. */
UmiStatus umi_designer_window_class_init(UmiDesignerAdaptiveWindowClass *window_class,
                                         const char *class_id,
                                         int32_t minimum_width,
                                         int32_t minimum_height,
                                         int detachable);
/* Return whether a logical viewport satisfies the window-class minimum bounds. */
int umi_designer_window_class_matches(const UmiDesignerAdaptiveWindowClass *window_class,
                                      UmiAdaptiveViewport viewport);

#ifdef __cplusplus
}
#endif
#endif
