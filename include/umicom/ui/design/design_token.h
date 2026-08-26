/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/design_token.h
 *
 * PURPOSE:
 *   Define typed design tokens that extend the existing generic Umicom theme-token model.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_DESIGN_TOKEN_H
#define INCLUDE_UMICOM_UI_DESIGN_DESIGN_TOKEN_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/color.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/color.h"
typedef struct UmiDesignToken { char id[UMI_DESIGN_ID_CAPACITY]; UmiDesignValueKind kind; UmiDesignRgba color; double number; int64_t integer; UmiDesignLength length; char text[UMI_DESIGN_TEXT_CAPACITY]; } UmiDesignToken;
/* Initialise a numeric design token. */
UmiStatus umi_design_token_number(UmiDesignToken *token, const char *id, double value);
/* Initialise a colour design token. */
UmiStatus umi_design_token_color(UmiDesignToken *token, const char *id, UmiDesignRgba color);
/* Initialise a bounded text design token. */
UmiStatus umi_design_token_text(UmiDesignToken *token, const char *id, const char *value);
/* Return one when the token identifier and typed value are valid. */
int umi_design_token_valid(const UmiDesignToken *token);

#ifdef __cplusplus
}
#endif

#endif
