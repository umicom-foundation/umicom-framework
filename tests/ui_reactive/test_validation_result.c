/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validation_result.c
 *
 * PURPOSE:
 *   Exercise the validation result reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_result.h"
int main(void) { UmiUiReactiveValidationResult item; umi_ui_reactive_validation_result_init(&item); return umi_ui_reactive_validation_result_valid(&item) ? 0 : 1; }
