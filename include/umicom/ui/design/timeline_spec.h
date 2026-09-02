/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/timeline_spec.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_TIMELINE_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_TIMELINE_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design timeline spec data shared with callers of this public contract.
 */
typedef struct UmiDesignTimelineSpec {
    uint16_t tracks;
    double pixels_per_unit;
    int snapping;
    int zoomable;
    int scrubbable;
} UmiDesignTimelineSpec;

/* Initialise the semantic timeline spec specification. */
UmiStatus umi_design_timeline_spec_init(UmiDesignTimelineSpec *spec, uint16_t tracks, double pixels_per_unit, int snapping, int zoomable, int scrubbable);
/* Return one when the semantic specification is internally consistent. */
int umi_design_timeline_spec_valid(const UmiDesignTimelineSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
