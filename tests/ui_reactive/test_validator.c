/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validator.c
 *
 * PURPOSE:
 *   Exercise the validator reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validator.h"
int main(void) { UmiUiReactiveValidator item; umi_ui_reactive_validator_init(&item); return umi_ui_reactive_validator_valid(&item) ? 0 : 1; }
