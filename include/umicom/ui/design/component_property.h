/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_property.h
 *
 * PURPOSE:
 *   Describe a typed configurable semantic component property for designers and property inspectors.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_PROPERTY_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_PROPERTY_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignComponentProperty { char id[UMI_DESIGN_ID_CAPACITY]; UmiDesignValueKind value_kind; int required; char default_token_id[UMI_DESIGN_ID_CAPACITY]; } UmiDesignComponentProperty;
/* Initialise a typed semantic component property descriptor. */
UmiStatus umi_design_component_property_init(UmiDesignComponentProperty *property, const char *id, UmiDesignValueKind value_kind, int required, const char *default_token_id);

#ifdef __cplusplus
}
#endif

#endif
