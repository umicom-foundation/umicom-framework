/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_fallback_chain.c
 *
 * PURPOSE:
 *   Maintain ordered semantic icon fallbacks when a renderer or platform lacks a preferred asset.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_fallback_chain.h"
#include <string.h>
UmiStatus umi_appearance_icon_fallback_chain_init(UmiAppearanceIconFallbackChain *chain){if(chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(chain,0,sizeof *chain);return UMI_STATUS_OK;}
UmiStatus umi_appearance_icon_fallback_chain_add(UmiAppearanceIconFallbackChain *chain,const char *icon_id){if(chain==NULL||!umi_appearance_id_valid(icon_id))return UMI_STATUS_INVALID_ARGUMENT;if(chain->count>=16U)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_appearance_copy_text(chain->ids[chain->count],sizeof chain->ids[chain->count],icon_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;chain->count++;return UMI_STATUS_OK;}
const char *umi_appearance_icon_fallback_chain_at(const UmiAppearanceIconFallbackChain *chain,size_t index){if(chain==NULL||index>=chain->count)return NULL;return chain->ids[index];}
