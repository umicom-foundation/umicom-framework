/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_variant.h
 *
 * PURPOSE:
 *   Define named semantic presentation variants without coupling variants to CSS classes or toolkit widget subclasses.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_VARIANT_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_VARIANT_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/semantic_role.h"
typedef struct UmiDesignComponentVariant { char id[UMI_DESIGN_ID_CAPACITY]; UmiDesignSemanticRole role; int outlined; int subtle; } UmiDesignComponentVariant;
/* Initialise a named semantic component variant. */
UmiStatus umi_design_component_variant_init(UmiDesignComponentVariant *variant, const char *id, UmiDesignSemanticRole role, int outlined, int subtle);

#ifdef __cplusplus
}
#endif

#endif
