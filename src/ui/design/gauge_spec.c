/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/gauge_spec.c
 *
 * PURPOSE:
 *   Define bounded gauge ranges, current values and warning thresholds for dashboard metrics.
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

#include "umicom/ui/design/gauge_spec.h"

#include <string.h>
int umi_design_gauge_spec_valid(const UmiDesignGaugeSpec *spec) { return spec!=NULL && (umi_design_number_valid(spec->minimum) && umi_design_number_valid(spec->maximum) && umi_design_number_valid(spec->value) && spec->maximum>spec->minimum && spec->value>=spec->minimum && spec->value<=spec->maximum && spec->warning_threshold>=spec->minimum && spec->warning_threshold<=spec->maximum && spec->danger_threshold>=spec->warning_threshold && spec->danger_threshold<=spec->maximum) ? 1 : 0; }
UmiStatus umi_design_gauge_spec_init(UmiDesignGaugeSpec *spec, double minimum, double maximum, double value, double warning_threshold, double danger_threshold)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->minimum=minimum;spec->maximum=maximum;spec->value=value;spec->warning_threshold=warning_threshold;spec->danger_threshold=danger_threshold;
    return umi_design_gauge_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
