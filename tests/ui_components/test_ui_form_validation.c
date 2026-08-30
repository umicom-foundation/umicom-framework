/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_form_validation.c
 *
 * PURPOSE:
 *   Test one reusable toolkit-neutral UI component behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/form_validation.h"
int main(void){UmiUiFormValidation a=umi_ui_form_validation_ok();UmiUiFormValidation b=umi_ui_form_validation_error("required");return a.valid&&!b.valid?0:1;}
