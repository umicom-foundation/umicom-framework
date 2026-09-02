/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/kpi_spec.c
 *
 * PURPOSE:
 *   Define dashboard KPI value, trend and semantic status presentation.
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

#include "umicom/ui/design/kpi_spec.h"

#include <string.h>
/* Check that design kpi spec satisfies its contract before another service relies on it. */
int umi_design_kpi_spec_valid(const UmiDesignKpiSpec *spec) { return spec!=NULL && (spec->label[0]!='\0' && umi_design_number_valid(spec->value) && umi_design_number_valid(spec->change) && spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT) ? 1 : 0; }
/*
 * Initialise design kpi spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_kpi_spec_init(UmiDesignKpiSpec *spec, const char *label, double value, double change, UmiDesignSemanticRole role, int percentage)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(spec->label, sizeof spec->label, label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->value = value;
    spec->change = change;
    spec->role = role;
    spec->percentage = percentage ? 1 : 0;
    return umi_design_kpi_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
