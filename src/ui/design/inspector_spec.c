/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/inspector_spec.c
 *
 * PURPOSE:
 *   Define searchable property-inspector grouping and advanced-property presentation.
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

#include "umicom/ui/design/inspector_spec.h"

#include <string.h>
int umi_design_inspector_spec_valid(const UmiDesignInspectorSpec *spec) { return spec!=NULL && (spec->category_count>0U && spec->category_count<=64U && spec->estimated_properties<=100000U) ? 1 : 0; }
UmiStatus umi_design_inspector_spec_init(UmiDesignInspectorSpec *spec, uint16_t category_count, size_t estimated_properties, int searchable, int advanced_toggle)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->category_count=category_count;spec->estimated_properties=estimated_properties;spec->searchable=searchable?1:0;spec->advanced_toggle=advanced_toggle?1:0;
    return umi_design_inspector_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
