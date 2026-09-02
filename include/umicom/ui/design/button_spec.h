/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/button_spec.h
 *
 * PURPOSE:
 *   Define button intent, size, label and icon semantics independent of frontend toolkit.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_BUTTON_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_BUTTON_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design button spec data shared with callers of this public contract.
 */
typedef struct UmiDesignButtonSpec {
    char label[UMI_DESIGN_TEXT_CAPACITY];
    UmiDesignSemanticRole role;
    UmiDesignDensity density;
    int icon_only;
    int destructive;
} UmiDesignButtonSpec;

/* Initialise the semantic button spec specification. */
UmiStatus umi_design_button_spec_init(UmiDesignButtonSpec *spec, const char *label, UmiDesignSemanticRole role, UmiDesignDensity density, int icon_only, int destructive);
/* Return one when the semantic specification is internally consistent. */
int umi_design_button_spec_valid(const UmiDesignButtonSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
