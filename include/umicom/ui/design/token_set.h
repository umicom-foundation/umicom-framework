/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/token_set.h
 *
 * PURPOSE:
 *   Maintain a bounded ordered set of typed design tokens with deterministic replacement semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_TOKEN_SET_H
#define INCLUDE_UMICOM_UI_DESIGN_TOKEN_SET_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/design_token.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/design_token.h"
/**
 * Represent the design token set data shared with callers of this public contract.
 */
typedef struct UmiDesignTokenSet { char id[UMI_DESIGN_ID_CAPACITY]; UmiDesignToken tokens[UMI_DESIGN_MAX_TOKENS]; size_t count; uint64_t revision; } UmiDesignTokenSet;
/* Initialise a named design-token set. */
UmiStatus umi_design_token_set_init(UmiDesignTokenSet *set, const char *id);
/* Insert or replace a token and advance the set revision. */
UmiStatus umi_design_token_set_upsert(UmiDesignTokenSet *set, const UmiDesignToken *token);
/* Resolve one token by stable identifier. */
UmiStatus umi_design_token_set_find(const UmiDesignTokenSet *set, const char *token_id, UmiDesignToken *out_token);

#ifdef __cplusplus
}
#endif

#endif
