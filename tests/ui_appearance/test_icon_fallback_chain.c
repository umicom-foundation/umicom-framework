/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_fallback_chain.c
 *
 * PURPOSE:
 *   Verify maintain ordered semantic icon fallbacks when a renderer or platform lacks a preferred asset.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAppearanceIconFallbackChain c;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_icon_fallback_chain_init(&c)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_icon_fallback_chain_add(&c,"generic.save")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(umi_appearance_icon_fallback_chain_at(&c,0),"generic.save")!=0)return 3;return 0;}
