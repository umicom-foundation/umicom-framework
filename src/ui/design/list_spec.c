/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/list_spec.c
 *
 * PURPOSE:
 *   Define virtualised list selection, estimated size and row-density semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/list_spec.h"

#include <string.h>
int umi_design_list_spec_valid(const UmiDesignListSpec *spec) { return spec!=NULL && (spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH && (spec->estimated_items==0U || spec->virtualised || spec->estimated_items<=1000U)) ? 1 : 0; }
UmiStatus umi_design_list_spec_init(UmiDesignListSpec *spec, size_t estimated_items, UmiDesignDensity density, int virtualised, int multi_select)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->estimated_items=estimated_items;spec->density=density;spec->virtualised=virtualised?1:0;spec->multi_select=multi_select?1:0;
    return umi_design_list_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
