/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/card_spec.h
 *
 * PURPOSE:
 *   Define reusable card elevation, interaction and semantic intent.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_CARD_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_CARD_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignCardSpec {
    UmiDesignSemanticRole role;
    uint8_t elevation_level;
    int interactive;
    int selected;
} UmiDesignCardSpec;

/* Initialise the semantic card spec specification. */
UmiStatus umi_design_card_spec_init(UmiDesignCardSpec *spec, UmiDesignSemanticRole role, uint8_t elevation_level, int interactive, int selected);
/* Return one when the semantic specification is internally consistent. */
int umi_design_card_spec_valid(const UmiDesignCardSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
