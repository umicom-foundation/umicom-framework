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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/alert_spec.h"

#include <string.h>
int umi_design_alert_spec_valid(const UmiDesignAlertSpec *spec) { return spec!=NULL && (spec->severity>=UMI_UI_SEVERITY_INFORMATION && spec->severity<=UMI_UI_SEVERITY_ERROR && spec->message[0]!='\0' ) ? 1 : 0; }
UmiStatus umi_design_alert_spec_init(UmiDesignAlertSpec *spec, UmiUiSeverity severity, const char *message, int dismissible, int actionable)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
        if (message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    spec->severity = severity;
    if (umi_design_copy_text(spec->message, sizeof spec->message, message) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->dismissible = dismissible ? 1 : 0;
    spec->actionable = actionable ? 1 : 0;
    return umi_design_alert_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
