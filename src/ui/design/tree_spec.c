/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/tree_spec.c
 *
 * PURPOSE:
 *   Define virtualised hierarchical tree depth, selection and checkbox semantics.
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

#include "umicom/ui/design/tree_spec.h"

#include <string.h>
/* Check that design tree spec satisfies its contract before another service relies on it. */
int umi_design_tree_spec_valid(const UmiDesignTreeSpec *spec) { return spec!=NULL && (spec->maximum_depth>0U && spec->maximum_depth<=64U) ? 1 : 0; }
/*
 * Initialise design tree spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_tree_spec_init(UmiDesignTreeSpec *spec, uint16_t maximum_depth, int virtualised, int multi_select, int checkboxes)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->maximum_depth=maximum_depth;spec->virtualised=virtualised?1:0;spec->multi_select=multi_select?1:0;spec->checkboxes=checkboxes?1:0;
    return umi_design_tree_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
