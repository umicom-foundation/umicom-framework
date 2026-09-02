/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/dialog_spec.c
 *
 * PURPOSE:
 *   Define dialog modality, sizing and governed action semantics.
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

#include "umicom/ui/design/dialog_spec.h"

#include <string.h>
/*
 * Check that design dialog spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_dialog_spec_valid(const UmiDesignDialogSpec *spec) { return spec!=NULL && (spec->width_class>=UMI_DESIGN_SIZE_COMPACT && spec->width_class<=UMI_DESIGN_SIZE_WIDE && spec->action_count>0U && spec->action_count<=8U && (!spec->destructive_action || spec->action_count>=2U)) ? 1 : 0; }
/*
 * Initialise design dialog spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_dialog_spec_init(UmiDesignDialogSpec *spec, UmiDesignSizeClass width_class, uint16_t action_count, int modal, int destructive_action)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->width_class=width_class;spec->action_count=action_count;spec->modal=modal?1:0;spec->destructive_action=destructive_action?1:0;
    return umi_design_dialog_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
