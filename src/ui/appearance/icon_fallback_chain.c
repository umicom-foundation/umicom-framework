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
/*
 * Initialise appearance icon fallback chain from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_appearance_icon_fallback_chain_init(UmiAppearanceIconFallbackChain *chain){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(chain,0,sizeof *chain);return UMI_STATUS_OK;}
/*
 * Add appearance icon fallback chain only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_appearance_icon_fallback_chain_add(UmiAppearanceIconFallbackChain *chain,const char *icon_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL||!umi_appearance_id_valid(icon_id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain->count>=16U)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(chain->ids[chain->count],sizeof chain->ids[chain->count],icon_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;chain->count++;return UMI_STATUS_OK;}
/*
 * Find appearance icon fallback chain while leaving the underlying catalogue or model
 * owned by this module.
 */
const char *umi_appearance_icon_fallback_chain_at(const UmiAppearanceIconFallbackChain *chain,size_t index){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL||index>=chain->count)return NULL;return chain->ids[index];}
