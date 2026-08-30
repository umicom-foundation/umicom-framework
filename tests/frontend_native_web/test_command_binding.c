/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_command_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web command binding.
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
#include "umicom/frontend/native_web/command_binding.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebCommandBinding b; CHECK(umi_native_web_command_binding_init(&b,"btn","build",true)==UMI_STATUS_OK); CHECK(b.enabled);
    return 0;
}
