/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/inspector_spec.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_INSPECTOR_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_INSPECTOR_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignInspectorSpec {
    uint16_t category_count;
    size_t estimated_properties;
    int searchable;
    int advanced_toggle;
} UmiDesignInspectorSpec;

/* Initialise the semantic inspector spec specification. */
UmiStatus umi_design_inspector_spec_init(UmiDesignInspectorSpec *spec, uint16_t category_count, size_t estimated_properties, int searchable, int advanced_toggle);
/* Return one when the semantic specification is internally consistent. */
int umi_design_inspector_spec_valid(const UmiDesignInspectorSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
