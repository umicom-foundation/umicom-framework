/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_stack.c
 *
 * PURPOSE:
 *   Store an ordered bounded semantic font stack for deterministic cross-platform fallback.
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
UmiStatus umi_appearance_font_stack_init(UmiAppearanceFontStack *stack,const char *stack_id){if(stack==NULL||!umi_appearance_id_valid(stack_id))return UMI_STATUS_INVALID_ARGUMENT;memset(stack,0,sizeof *stack);return umi_appearance_copy_text(stack->stack_id,sizeof stack->stack_id,stack_id);}
UmiStatus umi_appearance_font_stack_add(UmiAppearanceFontStack *stack,const char *family_id){if(stack==NULL||!umi_appearance_id_valid(family_id))return UMI_STATUS_INVALID_ARGUMENT;if(stack->count>=UMI_APPEARANCE_MAX_FONTS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_appearance_copy_text(stack->family_ids[stack->count],sizeof stack->family_ids[stack->count],family_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;stack->count++;return UMI_STATUS_OK;}
const char *umi_appearance_font_stack_at(const UmiAppearanceFontStack *stack,size_t index){if(stack==NULL||index>=stack->count)return NULL;return stack->family_ids[index];}
