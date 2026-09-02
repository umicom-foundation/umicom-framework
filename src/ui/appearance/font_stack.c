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
/*
 * Initialise appearance font stack from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_appearance_font_stack_init(UmiAppearanceFontStack *stack,const char *stack_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(stack==NULL||!umi_appearance_id_valid(stack_id))return UMI_STATUS_INVALID_ARGUMENT;memset(stack,0,sizeof *stack);return umi_appearance_copy_text(stack->stack_id,sizeof stack->stack_id,stack_id);}
/*
 * Add appearance font stack only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_appearance_font_stack_add(UmiAppearanceFontStack *stack,const char *family_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(stack==NULL||!umi_appearance_id_valid(family_id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(stack->count>=UMI_APPEARANCE_MAX_FONTS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(stack->family_ids[stack->count],sizeof stack->family_ids[stack->count],family_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;stack->count++;return UMI_STATUS_OK;}
/*
 * Find appearance font stack while leaving the underlying catalogue or model owned by this
 * module.
 */
const char *umi_appearance_font_stack_at(const UmiAppearanceFontStack *stack,size_t index){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(stack==NULL||index>=stack->count)return NULL;return stack->family_ids[index];}
