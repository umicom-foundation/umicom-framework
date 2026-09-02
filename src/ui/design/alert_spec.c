/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/alert_spec.c
 *
 * PURPOSE:
 *   Define inline and panel alert semantics with severity, dismissal and optional action.
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

#include "umicom/ui/design/alert_spec.h"

#include <string.h>
/* Check that design alert spec satisfies its contract before another service relies on it. */
int umi_design_alert_spec_valid(const UmiDesignAlertSpec *spec) { return spec!=NULL && (spec->severity>=UMI_UI_SEVERITY_INFORMATION && spec->severity<=UMI_UI_SEVERITY_ERROR && spec->message[0]!='\0' ) ? 1 : 0; }
/*
 * Initialise design alert spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_alert_spec_init(UmiDesignAlertSpec *spec, UmiUiSeverity severity, const char *message, int dismissible, int actionable)
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
        if (message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    spec->severity = severity;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(spec->message, sizeof spec->message, message) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->dismissible = dismissible ? 1 : 0;
    spec->actionable = actionable ? 1 : 0;
    return umi_design_alert_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
