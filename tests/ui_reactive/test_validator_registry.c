/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validator_registry.c
 *
 * PURPOSE:
 *   Exercise the validator registry reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validator_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveValidatorRegistry item; umi_ui_reactive_validator_registry_init(&item); return umi_ui_reactive_validator_registry_valid(&item) ? 0 : 1; }
