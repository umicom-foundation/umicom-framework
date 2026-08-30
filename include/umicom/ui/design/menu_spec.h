/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/menu_spec.h
 *
 * PURPOSE:
 *   Define scalable menu presentation, search and overflow semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_MENU_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_MENU_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignMenuSpec {
    uint16_t max_visible_items;
    int searchable;
    int icons;
    int accelerators;
} UmiDesignMenuSpec;

/* Initialise the semantic menu spec specification. */
UmiStatus umi_design_menu_spec_init(UmiDesignMenuSpec *spec, uint16_t max_visible_items, int searchable, int icons, int accelerators);
/* Return one when the semantic specification is internally consistent. */
int umi_design_menu_spec_valid(const UmiDesignMenuSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
