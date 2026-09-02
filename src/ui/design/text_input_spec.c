/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/text_input_spec.c
 *
 * PURPOSE:
 *   Define text-entry mode, placeholder and validation semantics for reusable inputs.
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

#include "umicom/ui/design/text_input_spec.h"

#include <string.h>
/*
 * Check that design text input spec satisfies its contract before another service relies
 * on it.
 */
int umi_design_text_input_spec_valid(const UmiDesignTextInputSpec *spec) { return spec!=NULL && (spec->max_length>0U && !(spec->password && spec->multiline)) ? 1 : 0; }
/*
 * Initialise design text input spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_text_input_spec_init(UmiDesignTextInputSpec *spec, const char *placeholder, uint32_t max_length, int password, int search, int multiline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->max_length=max_length;spec->password=password?1:0;spec->search=search?1:0;spec->multiline=multiline?1:0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(placeholder!=NULL){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_design_copy_text(spec->placeholder,sizeof spec->placeholder,placeholder)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;}
    return umi_design_text_input_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
