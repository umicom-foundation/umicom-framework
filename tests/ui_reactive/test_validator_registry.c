/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validator_registry.c
 *
 * PURPOSE:
 *   Exercise the validator registry reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validator_registry.h"
int main(void) { UmiUiReactiveValidatorRegistry item; umi_ui_reactive_validator_registry_init(&item); return umi_ui_reactive_validator_registry_valid(&item) ? 0 : 1; }
