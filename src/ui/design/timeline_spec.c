/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/timeline_spec.c
 *
 * PURPOSE:
 *   Define track, snapping and zoom semantics for media, audio, video and event timelines.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/timeline_spec.h"

#include <string.h>
/*
 * Check that design timeline spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_timeline_spec_valid(const UmiDesignTimelineSpec *spec) { return spec!=NULL && (spec->tracks>0U && spec->tracks<=512U && umi_design_number_valid(spec->pixels_per_unit) && spec->pixels_per_unit>0.0) ? 1 : 0; }
/*
 * Initialise design timeline spec from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_design_timeline_spec_init(UmiDesignTimelineSpec *spec, uint16_t tracks, double pixels_per_unit, int snapping, int zoomable, int scrubbable)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->tracks=tracks;spec->pixels_per_unit=pixels_per_unit;spec->snapping=snapping?1:0;spec->zoomable=zoomable?1:0;spec->scrubbable=scrubbable?1:0;
    return umi_design_timeline_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
