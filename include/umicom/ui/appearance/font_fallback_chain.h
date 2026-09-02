/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/font_fallback_chain.h
 *
 * PURPOSE:
 *   Model ordered script-aware fallback families without product-specific font discovery code.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_FONT_FALLBACK_CHAIN_H
#define UMICOM_UI_APPEARANCE_FONT_FALLBACK_CHAIN_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance font fallback entry data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceFontFallbackEntry { char family_id[UMI_APPEARANCE_ID_CAPACITY]; char script_id[UMI_APPEARANCE_ID_CAPACITY]; } UmiAppearanceFontFallbackEntry;
/**
 * Represent the appearance font fallback chain data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceFontFallbackChain { UmiAppearanceFontFallbackEntry items[UMI_APPEARANCE_MAX_FONTS]; size_t count; } UmiAppearanceFontFallbackChain;
/* Initialise an empty fallback chain. */
UmiStatus umi_appearance_font_fallback_chain_init(UmiAppearanceFontFallbackChain *chain);
/* Append one family for a script/language class. */
UmiStatus umi_appearance_font_fallback_chain_add(UmiAppearanceFontFallbackChain *chain,const char *family_id,const char *script_id);
/* Resolve the first family matching a requested script, then generic fallback. */
const char *umi_appearance_font_fallback_chain_resolve(const UmiAppearanceFontFallbackChain *chain,const char *script_id);

#ifdef __cplusplus
}
#endif
#endif
