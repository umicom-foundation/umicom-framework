/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/toolbar_spec.h
 *
 * PURPOSE:
 *   Define toolbar orientation, density and overflow behaviour for reusable command surfaces.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_TOOLBAR_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_TOOLBAR_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignToolbarSpec {
    UmiUiOrientation orientation;
    UmiDesignDensity density;
    uint16_t preferred_items;
    int overflow_menu;
} UmiDesignToolbarSpec;

/* Initialise the semantic toolbar spec specification. */
UmiStatus umi_design_toolbar_spec_init(UmiDesignToolbarSpec *spec, UmiUiOrientation orientation, UmiDesignDensity density, uint16_t preferred_items, int overflow_menu);
/* Return one when the semantic specification is internally consistent. */
int umi_design_toolbar_spec_valid(const UmiDesignToolbarSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
