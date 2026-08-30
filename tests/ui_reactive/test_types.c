/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_types.c
 *
 * PURPOSE:
 *   Exercise the types reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/types.h"
int main(void) { return umi_ui_reactive_identifier_valid("workspace.editor.selection") ? 0 : 1; }
