/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/badge_spec.h
 *
 * PURPOSE:
 *   Define compact status and metadata badges with semantic intent.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_BADGE_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_BADGE_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignBadgeSpec {
    char text[UMI_DESIGN_TEXT_CAPACITY];
    UmiDesignSemanticRole role;
    int outlined;
} UmiDesignBadgeSpec;

/* Initialise the semantic badge spec specification. */
UmiStatus umi_design_badge_spec_init(UmiDesignBadgeSpec *spec, const char *text, UmiDesignSemanticRole role, int outlined);
/* Return one when the semantic specification is internally consistent. */
int umi_design_badge_spec_valid(const UmiDesignBadgeSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
