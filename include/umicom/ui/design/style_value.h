/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/style_value.h
 *
 * PURPOSE:
 *   Represent a concrete style property as a typed design token value.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_STYLE_VALUE_H
#define INCLUDE_UMICOM_UI_DESIGN_STYLE_VALUE_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/design_token.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/design_token.h"
typedef struct UmiDesignStyleValue { char property_id[UMI_DESIGN_ID_CAPACITY]; UmiDesignToken value; } UmiDesignStyleValue;
/* Initialise one typed style property from a valid design token value. */
UmiStatus umi_design_style_value_init(UmiDesignStyleValue *style_value, const char *property_id, const UmiDesignToken *value);

#ifdef __cplusplus
}
#endif

#endif
