/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_fallback_chain.c
 *
 * PURPOSE:
 *   Model ordered script-aware fallback families without product-specific font discovery code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_fallback_chain.h"
#include <string.h>
/*
 * Initialise appearance font fallback chain from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_appearance_font_fallback_chain_init(UmiAppearanceFontFallbackChain *chain){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(chain,0,sizeof *chain);return UMI_STATUS_OK;}
/*
 * Add appearance font fallback chain only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_appearance_font_fallback_chain_add(UmiAppearanceFontFallbackChain *chain,const char *family_id,const char *script_id){UmiAppearanceFontFallbackEntry *e;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL||!umi_appearance_id_valid(family_id)||script_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain->count>=UMI_APPEARANCE_MAX_FONTS)return UMI_STATUS_CAPACITY_EXCEEDED;e=&chain->items[chain->count];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(e->family_id,sizeof e->family_id,family_id)!=UMI_STATUS_OK||umi_appearance_copy_text(e->script_id,sizeof e->script_id,script_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;chain->count++;return UMI_STATUS_OK;}
/*
 * Provide the appearance font fallback chain resolve operation used by this module and its
 * client applications.
 */
const char *umi_appearance_font_fallback_chain_resolve(const UmiAppearanceFontFallbackChain *chain,const char *script_id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL||chain->count==0U)return NULL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(script_id!=NULL)/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<chain->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(chain->items[i].script_id,script_id)==0)return chain->items[i].family_id;return chain->items[0].family_id;}
