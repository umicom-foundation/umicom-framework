/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_fallback_chain.c
 *
 * PURPOSE:
 *   Model ordered script-aware fallback families without product-specific font discovery code.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_fallback_chain.h"
#include <string.h>
UmiStatus umi_appearance_font_fallback_chain_init(UmiAppearanceFontFallbackChain *chain){if(chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(chain,0,sizeof *chain);return UMI_STATUS_OK;}
UmiStatus umi_appearance_font_fallback_chain_add(UmiAppearanceFontFallbackChain *chain,const char *family_id,const char *script_id){UmiAppearanceFontFallbackEntry *e;if(chain==NULL||!umi_appearance_id_valid(family_id)||script_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(chain->count>=UMI_APPEARANCE_MAX_FONTS)return UMI_STATUS_CAPACITY_EXCEEDED;e=&chain->items[chain->count];if(umi_appearance_copy_text(e->family_id,sizeof e->family_id,family_id)!=UMI_STATUS_OK||umi_appearance_copy_text(e->script_id,sizeof e->script_id,script_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;chain->count++;return UMI_STATUS_OK;}
const char *umi_appearance_font_fallback_chain_resolve(const UmiAppearanceFontFallbackChain *chain,const char *script_id){size_t i;if(chain==NULL||chain->count==0U)return NULL;if(script_id!=NULL)for(i=0;i<chain->count;i++)if(strcmp(chain->items[i].script_id,script_id)==0)return chain->items[i].family_id;return chain->items[0].family_id;}
