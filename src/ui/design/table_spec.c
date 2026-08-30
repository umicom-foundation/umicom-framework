/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/table_spec.c
 *
 * PURPOSE:
 *   Define enterprise table column, virtualisation, sorting, filtering and frozen-column semantics.
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

#include "umicom/ui/design/table_spec.h"

#include <string.h>
int umi_design_table_spec_valid(const UmiDesignTableSpec *spec) { return spec!=NULL && (spec->columns>0U && spec->frozen_columns<=spec->columns && spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH) ? 1 : 0; }
UmiStatus umi_design_table_spec_init(UmiDesignTableSpec *spec, uint16_t columns, uint16_t frozen_columns, UmiDesignDensity density, int virtualised, int sortable, int filterable)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->columns=columns;spec->frozen_columns=frozen_columns;spec->density=density;spec->virtualised=virtualised?1:0;spec->sortable=sortable?1:0;spec->filterable=filterable?1:0;
    return umi_design_table_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
