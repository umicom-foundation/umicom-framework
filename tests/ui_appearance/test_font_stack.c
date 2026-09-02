/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_stack.c
 *
 * PURPOSE:
 *   Verify store an ordered bounded semantic font stack for deterministic cross-platform fallback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_stack.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAppearanceFontStack s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_font_stack_init(&s,"ui")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_font_stack_add(&s,"font.primary")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(umi_appearance_font_stack_at(&s,0),"font.primary")!=0)return 3;return 0;}
