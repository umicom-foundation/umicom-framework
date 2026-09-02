/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_keyboard_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web keyboard binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/keyboard_binding.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebKeyboardBinding b; CHECK(umi_native_web_keyboard_binding_init(&b,"Ctrl+P","command.palette",true)==UMI_STATUS_OK); CHECK(strcmp(b.chord,"Ctrl+P")==0);
    return 0;
}
