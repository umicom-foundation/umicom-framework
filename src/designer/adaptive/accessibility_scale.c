/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/accessibility_scale.c
 *
 * PURPOSE:
 *   Combine user text, target-size and contrast preferences for adaptive design previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/accessibility_scale.h"
#include <stdint.h>
#include <limits.h>

/* Bound accessibility scaling to 0.75x-3.0x so preview geometry remains finite and useful. */
UmiStatus umi_designer_accessibility_scale_init(UmiDesignerAccessibilityScale *scale,
                                                double text_scale,
                                                double target_scale,
                                                int high_contrast)
{
    if(scale==NULL||!(text_scale>0.0)||!(target_scale>0.0))return UMI_STATUS_INVALID_ARGUMENT;
    scale->text_scale=text_scale<0.75?0.75:(text_scale>3.0?3.0:text_scale);
    scale->target_scale=target_scale<0.75?0.75:(target_scale>3.0?3.0:target_scale);
    scale->high_contrast=high_contrast!=0;
    return UMI_STATUS_OK;
}
/* Scale a logical target extent and round to the nearest integer. */
int32_t umi_designer_accessibility_scale_target(const UmiDesignerAccessibilityScale *scale,
                                                int32_t logical_extent)
{
    double value;
    if(scale==NULL||logical_extent<0)return -1;
    value=(double)logical_extent*scale->target_scale;
    if(value>(double)INT32_MAX)return INT32_MAX;
    return (int32_t)(value+0.5);
}
