/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/form_field_spec.c
 *
 * PURPOSE:
 *   Define labels, help text, required state and validation severity for reusable form fields.
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

#include "umicom/ui/design/form_field_spec.h"

#include <string.h>
/*
 * Check that design form field spec satisfies its contract before another service relies
 * on it.
 */
int umi_design_form_field_spec_valid(const UmiDesignFormFieldSpec *spec) { return spec!=NULL && (spec->label[0]!='\0' && spec->validation_severity>=UMI_UI_SEVERITY_INFORMATION && spec->validation_severity<=UMI_UI_SEVERITY_ERROR) ? 1 : 0; }
/*
 * Initialise design form field spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_form_field_spec_init(UmiDesignFormFieldSpec *spec, const char *label, const char *help_text, int required, UmiUiSeverity validation_severity)
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (help_text != NULL && umi_design_copy_text(spec->help_text, sizeof spec->help_text, help_text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->required = required ? 1 : 0;
    spec->validation_severity = validation_severity;
    return umi_design_form_field_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
