/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validation_result.c
 *
 * PURPOSE:
 *   Exercise the validation result reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_result.h"
int main(void) { UmiUiReactiveValidationResult item; umi_ui_reactive_validation_result_init(&item); return umi_ui_reactive_validation_result_valid(&item) ? 0 : 1; }
