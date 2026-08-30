/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validation_group.c
 *
 * PURPOSE:
 *   Exercise the validation group reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_group.h"
int main(void) { UmiUiReactiveValidationGroup item; umi_ui_reactive_validation_group_init(&item); return umi_ui_reactive_validation_group_valid(&item) ? 0 : 1; }
