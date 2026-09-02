/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/progress_spec.c
 *
 * PURPOSE:
 *   Define determinate and indeterminate progress semantics for tasks and background operations.
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

#include "umicom/ui/design/progress_spec.h"

#include <string.h>
/*
 * Check that design progress spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_progress_spec_valid(const UmiDesignProgressSpec *spec) { return spec!=NULL && (umi_design_number_valid(spec->minimum) && umi_design_number_valid(spec->maximum) && umi_design_number_valid(spec->value) && spec->maximum>spec->minimum && (spec->indeterminate || (spec->value>=spec->minimum && spec->value<=spec->maximum))) ? 1 : 0; }
/*
 * Initialise design progress spec from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_design_progress_spec_init(UmiDesignProgressSpec *spec, double minimum, double maximum, double value, int indeterminate, int show_text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->minimum=minimum;spec->maximum=maximum;spec->value=value;spec->indeterminate=indeterminate?1:0;spec->show_text=show_text?1:0;
    return umi_design_progress_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
