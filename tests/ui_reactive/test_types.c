/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_types.c
 *
 * PURPOSE:
 *   Exercise the types reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/types.h"
int main(void) { return umi_ui_reactive_identifier_valid("workspace.editor.selection") ? 0 : 1; }
