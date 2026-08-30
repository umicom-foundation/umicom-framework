/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/icon_fallback_chain.h
 *
 * PURPOSE:
 *   Maintain ordered semantic icon fallbacks when a renderer or platform lacks a preferred asset.
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
#ifndef UMICOM_UI_APPEARANCE_ICON_FALLBACK_CHAIN_H
#define UMICOM_UI_APPEARANCE_ICON_FALLBACK_CHAIN_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceIconFallbackChain { char ids[16][UMI_APPEARANCE_ID_CAPACITY]; size_t count; } UmiAppearanceIconFallbackChain;
/* Initialise an empty icon fallback chain. */
UmiStatus umi_appearance_icon_fallback_chain_init(UmiAppearanceIconFallbackChain *chain);
/* Append one semantic icon identity. */
UmiStatus umi_appearance_icon_fallback_chain_add(UmiAppearanceIconFallbackChain *chain,const char *icon_id);
/* Return a fallback icon identity by preference order. */
const char *umi_appearance_icon_fallback_chain_at(const UmiAppearanceIconFallbackChain *chain,size_t index);

#ifdef __cplusplus
}
#endif
#endif
