/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/font_stack.h
 *
 * PURPOSE:
 *   Store an ordered bounded semantic font stack for deterministic cross-platform fallback.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_FONT_STACK_H
#define UMICOM_UI_APPEARANCE_FONT_STACK_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceFontStack { char stack_id[UMI_APPEARANCE_ID_CAPACITY]; char family_ids[UMI_APPEARANCE_MAX_FONTS][UMI_APPEARANCE_ID_CAPACITY]; size_t count; } UmiAppearanceFontStack;
/* Initialise an empty semantic font stack. */
UmiStatus umi_appearance_font_stack_init(UmiAppearanceFontStack *stack,const char *stack_id);
/* Append one font-family identity while preserving declared preference order. */
UmiStatus umi_appearance_font_stack_add(UmiAppearanceFontStack *stack,const char *family_id);
/* Return a family identity by preference index. */
const char *umi_appearance_font_stack_at(const UmiAppearanceFontStack *stack,size_t index);

#ifdef __cplusplus
}
#endif
#endif
