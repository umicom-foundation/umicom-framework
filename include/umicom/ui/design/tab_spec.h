/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/tab_spec.h
 *
 * PURPOSE:
 *   Define document/tool tab close, pin, dirty-state and accent semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TAB_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_TAB_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignTabSpec {
    int closable;
    int pinnable;
    int dirty;
    int attention;
    UmiDesignSemanticRole accent_role;
} UmiDesignTabSpec;

/* Initialise the semantic tab spec specification. */
UmiStatus umi_design_tab_spec_init(UmiDesignTabSpec *spec, int closable, int pinnable, int dirty, int attention, UmiDesignSemanticRole accent_role);
/* Return one when the semantic specification is internally consistent. */
int umi_design_tab_spec_valid(const UmiDesignTabSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
