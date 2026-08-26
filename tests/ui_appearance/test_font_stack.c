/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_stack.c
 *
 * PURPOSE:
 *   Verify store an ordered bounded semantic font stack for deterministic cross-platform fallback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_stack.h"
#include <string.h>
int main(void){UmiAppearanceFontStack s;if(umi_appearance_font_stack_init(&s,"ui")!=UMI_STATUS_OK)return 1;if(umi_appearance_font_stack_add(&s,"font.primary")!=UMI_STATUS_OK)return 2;if(strcmp(umi_appearance_font_stack_at(&s,0),"font.primary")!=0)return 3;return 0;}
