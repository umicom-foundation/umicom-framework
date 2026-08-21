/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/ruler.c
 *
 * PURPOSE:
 *   Choose readable ruler intervals and project world coordinates through the
 *   current viewport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/ruler.h"
#include "internal.h"
#include <math.h>
#include <stdio.h>


void umi_workbench_designer_ruler_init(
    UmiWorkbenchDesignerRuler *ruler,
    UmiWorkbenchDesignerRulerAxis axis)
{
    if (ruler == NULL) return;
    (void)memset(ruler, 0, sizeof(*ruler));
    ruler->axis = axis;
}

static double ruler_nice_step(double requested)
{
    double exponent;
    double fraction;
    double base;
    if (requested <= 0.0) return 1.0;
    exponent = floor(log10(requested));
    base = pow(10.0, exponent);
    fraction = requested / base;
    if (fraction <= 1.0) return base;
    if (fraction <= 2.0) return 2.0 * base;
    if (fraction <= 5.0) return 5.0 * base;
    return 10.0 * base;
}

UmiStatus umi_workbench_designer_ruler_build(
    UmiWorkbenchDesignerRuler *ruler,
    const UmiWorkbenchDesignerViewport *viewport,
    double screen_extent,
    double preferred_minor_pixels)
{
    double world_start;
    double world_end;
    double value;
    if (ruler == NULL || viewport == NULL || screen_extent <= 0.0 ||
        preferred_minor_pixels <= 0.0 || viewport->zoom <= 0.0 ||
        (ruler->axis != UMI_WORKBENCH_DESIGNER_RULER_HORIZONTAL &&
         ruler->axis != UMI_WORKBENCH_DESIGNER_RULER_VERTICAL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ruler->count = 0U;
    ruler->minor_step = ruler_nice_step(
        preferred_minor_pixels / viewport->zoom);
    ruler->major_step = ruler->minor_step * 5.0;
    if (ruler->axis == UMI_WORKBENCH_DESIGNER_RULER_HORIZONTAL) {
        world_start = viewport->origin.x;
        world_end = viewport->origin.x + screen_extent / viewport->zoom;
    } else {
        world_start = viewport->origin.y;
        world_end = viewport->origin.y + screen_extent / viewport->zoom;
    }
    value = floor(world_start / ruler->minor_step) * ruler->minor_step;
    while (value <= world_end &&
           ruler->count < UMI_WORKBENCH_DESIGNER_MAX_RULER_TICKS) {
        UmiWorkbenchDesignerRulerTick *tick = &ruler->ticks[ruler->count];
        const double major_remainder = fmod(fabs(value), ruler->major_step);
        tick->world_value = value;
        tick->screen_position = (value - world_start) * viewport->zoom;
        tick->major = major_remainder < 0.000001 ||
            fabs(major_remainder - ruler->major_step) < 0.000001;
        if (tick->major) {
            (void)snprintf(tick->label, sizeof(tick->label), "%.0f", value);
        } else {
            tick->label[0] = '\0';
        }
        ruler->count += 1U;
        value += ruler->minor_step;
    }
    ruler->viewport_revision = viewport->revision;
    ruler->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerRulerTick *umi_workbench_designer_ruler_at(
    const UmiWorkbenchDesignerRuler *ruler,
    size_t index)
{
    return ruler != NULL && index < ruler->count ? &ruler->ticks[index] : NULL;
}
