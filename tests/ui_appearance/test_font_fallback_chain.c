/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_fallback_chain.c
 *
 * PURPOSE:
 *   Verify model ordered script-aware fallback families without product-specific font discovery code.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAppearanceFontFallbackChain c;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_font_fallback_chain_init(&c)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_font_fallback_chain_add(&c,"font.latin","latin")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(umi_appearance_font_fallback_chain_resolve(&c,"latin"),"font.latin")!=0)return 3;return 0;}
