/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_fallback_chain.c
 *
 * PURPOSE:
 *   Verify maintain ordered semantic icon fallbacks when a renderer or platform lacks a preferred asset.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_fallback_chain.h"
#include <string.h>
int main(void){UmiAppearanceIconFallbackChain c;if(umi_appearance_icon_fallback_chain_init(&c)!=UMI_STATUS_OK)return 1;if(umi_appearance_icon_fallback_chain_add(&c,"generic.save")!=UMI_STATUS_OK)return 2;if(strcmp(umi_appearance_icon_fallback_chain_at(&c,0),"generic.save")!=0)return 3;return 0;}
