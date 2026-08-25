/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_keyboard_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web keyboard binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/keyboard_binding.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebKeyboardBinding b; CHECK(umi_native_web_keyboard_binding_init(&b,"Ctrl+P","command.palette",true)==UMI_STATUS_OK); CHECK(strcmp(b.chord,"Ctrl+P")==0);
    return 0;
}
