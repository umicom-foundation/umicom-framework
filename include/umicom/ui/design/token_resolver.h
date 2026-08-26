/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/token_resolver.h
 *
 * PURPOSE:
 *   Resolve design tokens through component, application and system layers with explicit precedence.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TOKEN_RESOLVER_H
#define INCLUDE_UMICOM_UI_DESIGN_TOKEN_RESOLVER_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/token_set.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/token_set.h"
typedef struct UmiDesignTokenResolver { const UmiDesignTokenSet *system_tokens; const UmiDesignTokenSet *application_tokens; const UmiDesignTokenSet *component_tokens; } UmiDesignTokenResolver;
/* Resolve a token using component then application then system precedence. */
UmiStatus umi_design_token_resolver_resolve(const UmiDesignTokenResolver *resolver, const char *token_id, UmiDesignToken *out_token, uint8_t *out_layer);

#ifdef __cplusplus
}
#endif

#endif
